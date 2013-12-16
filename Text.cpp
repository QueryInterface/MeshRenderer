#include "Text.h"
#include "RenderContext.h"
#include "Sprite.h"
#include <direct.h>

/// Letter

/// Line
Line::Line(Text* parent) : m_parent(parent) {
}

/// Text
Text::Text(RenderContext* renderContext, std::string& fontPath, float fontSize, IRenderable* parent) 
    : m_renderContext(renderContext)
    , m_prerenderedText(renderContext)
    , m_prerendered(true)
    , m_textAdjusted(false)
    , m_textPosition(0, 0)
    , m_lineInterval(0)
    , m_curLine(this) {
    FT_Library fontLibrary;
    if(FT_Init_FreeType(&fontLibrary)) {
        throw std::runtime_error("Failed to create FreeType object");
    }
    if(FT_New_Face(fontLibrary, fontPath.c_str(), 0, &m_fontFace)) {
        throw std::runtime_error("Failed to create FreeType font face");
    }
    // Set default values for font rendering
    m_curLetter.m_color = Vector3<float>(1.0f, 1.0f, 1.0f);
    m_curLetter.m_letter = 0;
    m_curLetter.m_offsetX = 0;
    m_curLetter.m_size = 12;
    m_curLine.m_offsetY = 0;
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
    if (!m_prerendered) {
        prerenderText();
    }
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
        // Process special symbols
        switch (*text) {
        case '\n':
            {
                // Store symbol
                m_curLine.m_letters.push_back(m_curLetter);
                // Compelte line
                m_text.push_back(std::move(m_curLine));
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
                m_curLine.m_letters.push_back(m_curLetter);
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
        l.m_sprite->SetPosition(m_curLetter.m_offsetX + glyphDesc.Left, m_curLine.m_offsetY - glyphDesc.Top);
        l.m_sprite->SetColor(l.m_color.x, l.m_color.y, l.m_color.z);
        m_curLetter.m_offsetX += glyphDesc.Advance;
        m_curLine.m_letters.push_back(l);
        text++;
    }
}

void Text::prerenderText() {
    ////if (m_textData.PrerenderedText) return;

    //m_textData.BoundingRect.Left = m_textData.BoundingRect.Top = 1.0f;
    //m_textData.BoundingRect.Right = m_textData.BoundingRect.Bottom = 0.0f;
    //for (auto letter : m_textData.Text) {
    //    shared_ptr<Sprite>& sprite = letter.second;
    //    if (sprite->GetX() < m_textData.BoundingRect.Left) m_textData.BoundingRect.Left = sprite->GetX();
    //    if (sprite->GetY() < m_textData.BoundingRect.Top) m_textData.BoundingRect.Top = sprite->GetY();
    //    if (sprite->GetX() + sprite->GetWidth()> m_textData.BoundingRect.Right) m_textData.BoundingRect.Right = sprite->GetX() + sprite->GetWidth();
    //    if (sprite->GetY() + sprite->GetHeight() > m_textData.BoundingRect.Bottom) m_textData.BoundingRect.Bottom = sprite->GetY() + sprite->GetHeight(); 
    //};
    //// Create a new copy of glyphs in space of bounding box
    //float width_n = (m_textData.BoundingRect.Right - m_textData.BoundingRect.Left);
    //float height_n = (m_textData.BoundingRect.Bottom - m_textData.BoundingRect.Top);
    //uint32_t width = width_n * m_parent->GetWidth();
    //uint32_t height = height_n * m_parent->GetHeight();
    //std::list<Sprite> glyphsCopy = m_textData.Glyphs;
    //std::for_each(glyphsCopy.begin(), glyphsCopy.end(), [&](Sprite& sprite) {
    //    sprite->SetPosition((sprite->GetPosX() - m_textData.BoundingRect.Left) / width_n, (sprite->GetPosY() - m_textData.BoundingRect.Top) / height_n);
    //    sprite->SetSize(sprite->GetWidth() / width_n, sprite->GetHeight() / height_n);
    //});
    //CComPtr<IDirect3DTexture9> tex = m_renderContext->CreateRenderTarget(width, height, D3DFMT_A8R8G8B8);
    //m_renderContext->SetRenderTarget(tex);
    //m_renderContext->Clear();
    //// Render sprites
    ////D3DVIEWPORT9 prevViewPort;
    ////D3DVIEWPORT9 newViewPort;
    ////newViewPort.X = 0;
    ////newViewPort.Y = 0;
    ////newViewPort.Width = width;
    ////newViewPort.Height = height;
    ////newViewPort.MinZ = 0.0f;
    ////newViewPort.MaxZ = 1.0f;
    ////m_renderContext->Device->GetViewport(&prevViewPort);
    ////m_renderContext->Device->SetViewport(&newViewPort);
    //std::for_each(glyphsCopy.begin(), glyphsCopy.end(), [&](Sprite sprite) {
    //    sprite->Render();
    //});
    ////m_renderContext->Device->SetViewport(&prevViewPort);
    //m_renderContext->SetRenderTarget(m_renderContext->DefaultRT);
    //m_textData.PrerenderedText = tex;
}

void Text::adjustText() {
    uint32_t curLineHeight = 0;
    for (Line& line : m_text) {
        uint32_t lineHeight = 0;
        for (const Letter& letter : line.m_letters) {
            if (letter.GetSize() > lineHeight) lineHeight = letter.GetSize();
        }
        curLineHeight += lineHeight + m_lineInterval;
        line.m_offsetY = curLineHeight;
    }
    m_textAdjusted = true;
}