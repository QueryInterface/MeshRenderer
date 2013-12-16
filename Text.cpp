#include "Text.h"
#include "RenderContext.h"
#include "Sprite.h"
#include <direct.h>

/// Letter

/// Line
Line::Line(Text* parent) : m_parent(parent) {
}

/// Text
Text::Text(RenderContext* renderContext, std::string& fontPath) 
    : m_renderContext(renderContext)
    , m_prerenderedText(renderContext)
    , m_prerendered(true)
    , m_textAdjusted(false)
    , m_lineInterval(0)
    , m_curLine(NULL)
    , m_boundWidth(0) 
    , m_boundHeight(0) {
    FT_Library fontLibrary;
    if(FT_Init_FreeType(&fontLibrary)) {
        throw std::runtime_error("Failed to create FreeType object");
    }
    if(FT_New_Face(fontLibrary, fontPath.c_str(), 0, &m_fontFace)) {
        throw std::runtime_error("Failed to create FreeType font face");
    }
    // Set default values for font rendering
    m_prerenderedText.SetPosition(0, 0);
    m_curLetter.m_color = Vector3<float>(1.0f, 1.0f, 1.0f);
    m_curLetter.m_letter = 0;
    m_curLetter.m_offsetX = 0;
    m_curLetter.m_size = 12;
    m_curLetter.m_height = 0;
    m_curLetter.m_width = 0;

    Line line(this);
    m_text.push_back(line);
    m_curLine = &m_text.back();
    m_curLine->m_offsetY = 0;
    FT_Set_Pixel_Sizes(m_fontFace, 0, m_curLetter.m_size);
}

Text::~Text() {
}

void Text::SetLineInterval(uint32_t interval) {
    m_lineInterval = interval;
    if (m_textAdjusted) m_textAdjusted = false;
}

void Text::Clear() {
    m_text.clear();	
    m_prerendered = false;
}

void Text::Render() {
    if (!m_textAdjusted) {
        adjustText();
    }
    //if (!m_prerendered) {
        prerenderText();
    //}
    m_prerenderedText.Render();
}

Text& Text::operator<<(const set_size& token) {
    m_curLetter.m_size = token.m_size;
    FT_Set_Pixel_Sizes(m_fontFace, 0, m_curLetter.m_size);
    return *this;
}

Text& Text::operator<<(const set_color& token) {
    m_curLetter.m_color = std::move(token.m_color);
    return *this;
}

Text& Text::operator<<(const char* text) {
    if(text) {
        processString(text);
        if (m_prerendered) m_prerendered = false;
        if (m_textAdjusted) m_textAdjusted = false;
    }
    return *this;
}

Text& Text::operator<<(const std::string& text) {
    if (!text.empty()) {
        processString(text.c_str());
        if (m_prerendered) m_prerendered = false;
        if (m_textAdjusted) m_textAdjusted = false;
    }
    return *this;
}

bool Text::getGlyphDesc(uint8_t c, GlyphDesc& glyphDesc) {
    if (!FT_Load_Char(m_fontFace, c, FT_LOAD_RENDER)) {
        glyphDesc.Width = m_fontFace->glyph->bitmap.width;
        glyphDesc.Height = m_fontFace->glyph->bitmap.rows;
        glyphDesc.Advance = m_fontFace->glyph->advance.x >> 6;
        glyphDesc.Left = m_fontFace->glyph->bitmap_left;
        glyphDesc.Top = m_fontFace->glyph->bitmap_top;
        // Skip texture creation for non-visible symbols
        if (glyphDesc.Width && glyphDesc.Height) {
            CHECK(m_renderContext->GetDevice()->CreateTexture(glyphDesc.Width, glyphDesc.Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &glyphDesc.Texture, NULL), "Failed to create glyph texture");
            D3DLOCKED_RECT rect;
            glyphDesc.Texture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);
            uint8_t* src = m_fontFace->glyph->bitmap.buffer;
            uint8_t* dst = (uint8_t*)rect.pBits;
            for (uint32_t i = 0; i < glyphDesc.Height; i++) {
                for (uint32_t j = 0; j < glyphDesc.Width; j++) {
                    dst[4*j] = src[j];
                    dst[4*j + 1] = src[j];
                    dst[4*j + 2] = src[j];
                    dst[4*j + 3] = src[j];
                }
                dst += rect.Pitch;
                src += glyphDesc.Width;
            }
            glyphDesc.Texture->UnlockRect(0);
        }
        return true;
    }
    return false;
}

void Text::processString(const char* text) {
    while (*text) {
        GlyphDesc glyphDesc;
        if (!getGlyphDesc(*text, glyphDesc)) {
            continue;
        };
        m_curLetter.m_letter = *text;
        m_curLetter.m_width = glyphDesc.Width;
        m_curLetter.m_height = glyphDesc.Height;
        // Process special symbols
        switch (*text) {
        case '\n':
            {
                // Store symbol
                m_curLine->m_letters.push_back(m_curLetter);
                // Create new line
                Line line(this);
                m_text.push_back(line);
                m_curLine = &m_text.back();
                m_curLetter.m_offsetX = 0;
                m_curLetter.m_letter = 0;
                ++text;
                continue;
            }
            break;
        case ' ':
            {
                // Store symbol
                m_curLetter.m_letter = *text;
                m_curLine->m_letters.push_back(m_curLetter);
                // Complete line
                m_curLetter.m_offsetX += glyphDesc.Advance;
                ++text;
                continue;
            }
            break;
        }

        // Render glyph texture to font texture
        Letter l = m_curLetter;
        l.m_sprite = std::make_shared<Sprite>(m_renderContext);
        Rect texRect;
        texRect.Left = 0;
        texRect.Right = 1.0f;
        texRect.Top = 0;
        texRect.Bottom = 1.0f;
        l.m_sprite->SetSize(glyphDesc.Width, glyphDesc.Height);
        l.m_sprite->SetTextureCoords(texRect);
        l.m_sprite->SetTexture(glyphDesc.Texture);
        l.m_sprite->SetPosition(m_curLetter.m_offsetX + glyphDesc.Left, m_curLine->m_offsetY - glyphDesc.Top);
        l.m_sprite->SetColor(l.m_color.x, l.m_color.y, l.m_color.z);
        m_curLetter.m_offsetX += glyphDesc.Advance;
        m_curLine->m_letters.push_back(l);
        text++;
    }
}

void Text::prerenderText() {
    //if (m_prerendered) return;

    RenderTarget rt = m_renderContext->CreateRenderTarget(m_boundWidth, m_boundHeight, D3DFMT_A8R8G8B8);
    rt.MakeCurrent();
    m_renderContext->Clear();
    // Render sprites
    for (Line& line : m_text) {
        for (const Letter& letter : line.m_letters) {
            if (letter.m_sprite) {
                letter.m_sprite->Render();
            }
        }
    }
    m_renderContext->GetDefaultRT().MakeCurrent();
    m_prerenderedText.SetSize(m_boundWidth, m_boundHeight);
    m_prerenderedText.SetTexture(rt.GetTexture());
    m_prerendered = true;
}

void Text::adjustText() {
    if (m_textAdjusted) return;

    uint32_t totalLineHeight = 0;
    uint32_t curLineHeight = 0;
    uint32_t maxLineLength = 0;
    for (Line& line : m_text) {
        curLineHeight = 0;
        for (const Letter& letter : line.m_letters) {
            if (letter.GetSize() > curLineHeight) curLineHeight = letter.GetSize();
            if (letter.GetOffsetX() + letter.GetWidth() > maxLineLength) maxLineLength = letter.GetOffsetX() + letter.GetWidth();
        }
        totalLineHeight += curLineHeight + m_lineInterval;
        line.m_offsetY = totalLineHeight;
        for (const Letter& letter : line.m_letters) {
            if (letter.m_sprite) {
                letter.m_sprite->SetPosition(letter.m_sprite->GetX(), letter.m_sprite->GetY() + totalLineHeight);
            }
        }
    }
    m_boundHeight = totalLineHeight + curLineHeight;
    m_boundWidth = maxLineLength;
    m_textAdjusted = true;
}