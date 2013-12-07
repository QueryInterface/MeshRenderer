#include "Font.h"
#include "RenderContext.h"
#include "Sprite.h"
#include <direct.h>

#define FONT_TEX_WIDTH  512
#define FONT_TEX_HEIGHT 512

Font::Font(RenderContext* renderContext, std::string& fontPath) 
    : m_renderContext(renderContext)
    , m_fontSize(12)
    , m_position(0.0f, 0.0f) {
    if(FT_Init_FreeType(&m_fontLibrary)) {
        throw std::runtime_error("Failed to create FreeType object");
    }
    if(FT_New_Face(m_fontLibrary, fontPath.c_str(), 0, &m_fontFace)) {
        throw std::runtime_error("Failed to create FreeType font face");
    }
    setFontSize(m_fontSize);
}

Font::~Font() {
}

void Font::Clear() {
    m_text.Text.clear();	
    m_text.CharToGlyph.clear();
    m_text.Glyphs.clear();
}

void Font::Render() {
    //std::for_each(m_text.Glyphs.begin(), m_text.Glyphs.end(), [](Sprite& s) {s.Render();});
    Sprite sprite(m_renderContext, RM_ALPHA_TO_COLOR);
    prerenderText();
    sprite.SetPosition(m_text.BoundingRect.Left, m_text.BoundingRect.Top);
    sprite.SetSize(m_text.BoundingRect.Right - m_text.BoundingRect.Left, m_text.BoundingRect.Bottom - m_text.BoundingRect.Top);
    sprite.SetTexture(m_text.PrerenderedText);
    sprite.Render();
}

inline Font::Iterator& Font::Begin() {
    return m_text.Text.begin();
}

inline Font::Iterator& Font::End() {
    return m_text.Text.end();
}

inline Font::Iterator& Font::Erase(Iterator& iter) {
    // TODO: Optimize so erase is performed with constant time
    Iterator ret = m_text.Text.erase(iter);
    m_text.Glyphs.erase(m_text.CharToGlyph[iter._Ptr]);
    prerenderText();
    return ret;
}

Font& Font::operator<<(const set_size& token) {
    setFontSize(token.m_size);
    return *this;
}

Font& Font::operator<<(const set_pos&  token) {
    m_position = token.m_pos;
    return *this;
}

Font& Font::operator<<(const char* text) {
    processString(text);
    return *this;
}

Font& Font::operator<<(const std::string& text) {
    processString(text.c_str());
    return *this;
}

void Font::setFontSize(uint32_t size) {
    // Update size
    m_fontSize = size;
    FT_Set_Pixel_Sizes(m_fontFace, 0, m_fontSize);
}

void Font::processString(const char* text) {
    while (*text) {
        switch (*text) {
        case '\n':
            {
                m_position.y += (float)m_fontSize / m_renderContext->GetWindowHeight();
                m_position.x = 0;
                m_text.Text += *text;
                ++text;
                continue;
            }
            break;
        case ' ':
            {
                m_position.x += (float)m_fontSize / (2 * m_renderContext->GetWindowWidth());
                m_text.Text += *text;
                ++text;
                continue;
            }
            break;
        }

        if(!FT_Load_Char(m_fontFace, *text, FT_LOAD_RENDER)) {
            CComPtr<IDirect3DTexture9> glyphTexture;
            CComPtr<IDirect3DSurface9> glyphSurface;
            // Calculate glyph parameters
            uint32_t width = m_fontFace->glyph->bitmap.width;
            uint32_t height = m_fontFace->glyph->bitmap.rows;
            float	 width_n = (float)width / m_renderContext->GetWindowWidth();
            float	 height_n = (float)height / m_renderContext->GetWindowHeight();
            uint32_t size = width * height;
            // Create glyph texture
            CHECK(m_renderContext->Device->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8, D3DPOOL_DEFAULT, &glyphTexture, NULL), "Failed to create glyph texture");
            D3DLOCKED_RECT rect;
            glyphTexture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);
            uint8_t* src = m_fontFace->glyph->bitmap.buffer;
            uint8_t* dst = (uint8_t*)rect.pBits;
            for (uint32_t i = 0; i < height; i++) {
                memcpy(dst, src, width);
                dst += rect.Pitch;
                src += width;
            }
            memcpy(rect.pBits, m_fontFace->glyph->bitmap.buffer, size);
            glyphTexture->UnlockRect(0);
            // Render glyph texture to font texture
            Rect texRect;
            texRect.Left = 0;
            texRect.Right = 1.0f;
            texRect.Top = 0;
            texRect.Bottom = 1.0f;
            Sprite glyph(m_renderContext);
            glyph.SetSize(width_n, height_n);
            glyph.SetTextureCoords(texRect);
            glyph.SetTexture(glyphTexture);
            glyph.SetPosition(m_position.x, m_position.y - (float)m_fontFace->glyph->bitmap_top / FONT_TEX_WIDTH);
            m_position.x += width_n + (float)m_fontFace->glyph->bitmap_left / FONT_TEX_HEIGHT;
            m_text.Glyphs.push_back(std::move(glyph));
            m_text.Text += *text;	
            m_text.CharToGlyph[(--m_text.Text.end())._Ptr] = --m_text.Glyphs.end();
            text++;
        }
        if (m_text.PrerenderedText) m_text.PrerenderedText = NULL; // prerendered data is invalid so we clear it
    }
}

void Font::prerenderText() {
    //if (m_text.PrerenderedText) return;

    m_text.BoundingRect.Left = m_text.BoundingRect.Top = 1.0f;
    m_text.BoundingRect.Right = m_text.BoundingRect.Bottom = 0.0f;
    std::for_each(m_text.Glyphs.begin(), m_text.Glyphs.end(), [&](Sprite sprite) {
        const Sprite::Desc& desc = sprite.GetDesc();
        if (desc.PosX < m_text.BoundingRect.Left) m_text.BoundingRect.Left = desc.PosX;
        if (desc.PosY < m_text.BoundingRect.Top) m_text.BoundingRect.Top = desc.PosY;
        if (desc.PosX + desc.Width > m_text.BoundingRect.Right) m_text.BoundingRect.Right = desc.PosX + desc.Width;
        if (desc.PosY + desc.Height > m_text.BoundingRect.Bottom) m_text.BoundingRect.Bottom = desc.PosY + desc.Height; 
    });
    // Create a new copy of glyphs in space of bounding box
    float width_n = (m_text.BoundingRect.Right - m_text.BoundingRect.Left);
    float height_n = (m_text.BoundingRect.Bottom - m_text.BoundingRect.Top);
    uint32_t width = width_n * m_renderContext->GetWindowWidth();
    uint32_t height = height_n * m_renderContext->GetWindowHeight();
    std::list<Sprite> glyphsCopy = m_text.Glyphs;
    std::for_each(glyphsCopy.begin(), glyphsCopy.end(), [&](Sprite& sprite) {
        const Sprite::Desc& desc = sprite.GetDesc();
        sprite.SetPosition((desc.PosX - m_text.BoundingRect.Left) / width_n, (desc.PosY - m_text.BoundingRect.Top) / height_n);
        sprite.SetSize(desc.Width / width_n, desc.Height / height_n);
    });
    CComPtr<IDirect3DTexture9> tex = m_renderContext->CreateRenderTarget(width, height, D3DFMT_A8);
    m_renderContext->SetRenderTarget(tex);
    m_renderContext->Clear();
    // Render sprites
    std::for_each(glyphsCopy.begin(), glyphsCopy.end(), [&](Sprite sprite) {
        sprite.Render();
    });
    m_renderContext->SetRenderTarget(m_renderContext->DefaultRT);
    m_text.PrerenderedText = tex;
}