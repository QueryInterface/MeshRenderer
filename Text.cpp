#include "Text.h"
#include "RenderContext.h"
#include "Sprite.h"
#include <direct.h>

#define MAX_FONT_SIZE 64

Text::Text(RenderContext* renderContext, std::string& fontPath, float fontSize, IRenderable* parent) 
    : m_renderContext(renderContext)
    , m_parent(parent)
    , m_prerenderedText(renderContext)
    , m_prerenderIsDirty(true) {
    FT_Library fontLibrary;
    if(FT_Init_FreeType(&fontLibrary)) {
        throw std::runtime_error("Failed to create FreeType object");
    }
    if(FT_New_Face(fontLibrary, fontPath.c_str(), 0, &m_fontFace)) {
        throw std::runtime_error("Failed to create FreeType font face");
    }
    FT_Set_Pixel_Sizes(m_fontFace, 0, MAX_FONT_SIZE);
}

Text::~Text() {
}

void Text::Clear() {
    m_text.clear();	
}

void Text::Render() {
    if (m_prerenderIsDirty) {
        prerenderText();
    }
    m_prerenderedText.Render();
}

inline Text::Iterator Text::Begin() const {
    return m_text.begin();
}

inline Text::Iterator Text::End() const {
    return m_text.end();
}

inline Text::Iterator Text::Erase(Iterator& iter) {
    // TODO: Optimize so erase is performed with constant time
    Iterator ret = m_text.erase(iter.m_iter);
    m_prerenderIsDirty = true;
    return ret;
}

Text& Text::operator<<(const set_size& token) {
    m_curFontDesc.Size = token.m_size;
    FT_Set_Pixel_Sizes(m_fontFace, 0, m_curFontDesc.Size);
    return *this;
}

//Text& Text::operator<<(const set_pos&  token) {
//    m_curFontDesc.Position = std::move(token.m_pos);
//    return *this;
//}

Text& Text::operator<<(const set_color& token) {
    m_curFontDesc.Color = std::move(token.m_color);
    return *this;
}

Text& Text::operator<<(const char* text) {
    processString(text);
    return *this;
}

Text& Text::operator<<(const std::string& text) {
    processString(text.c_str());
    return *this;
}

const CComPtr<IDirect3DTexture9>& Text::getGlyphTexture(uint8_t c) {
    auto glyphTextureIter = m_glyphTextures.find(c);
    if (glyphTextureIter != m_glyphTextures.end()) {
        return glyphTextureIter->second;
    }
    else {
        if (!FT_Load_Char(m_fontFace, c, FT_LOAD_RENDER)) {
            CComPtr<IDirect3DTexture9>& glyphTexture = m_glyphTextures[c];
            uint32_t width = m_fontFace->glyph->bitmap.width;
            uint32_t height = m_fontFace->glyph->bitmap.rows;
            CHECK(m_renderContext->Device->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &glyphTexture, NULL), "Failed to create glyph texture");
            D3DLOCKED_RECT rect;
            glyphTexture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);
            uint8_t* src = m_fontFace->glyph->bitmap.buffer;
            uint8_t* dst = (uint8_t*)rect.pBits;
            for (uint32_t i = 0; i < height; i++) {
                for (uint32_t j = 0; j < width; j++) {
                    dst[4*j] = src[j];
                    dst[4*j + 1] = src[j];
                    dst[4*j + 2] = src[j];
                    dst[4*j + 3] = src[j];
                }
                dst += rect.Pitch;
                src += width;
            }
            glyphTexture->UnlockRect(0);
            return glyphTexture;
        }
    }
    return nullptr;
}

void Text::processString(const char* text) {
    while (*text) {
        switch (*text) {
        case '\n':
            {
                m_curFontDesc.Position.y += (float)m_curFontDesc.Size / m_parent->GetWidth();
                m_curFontDesc.Position.x = 0;
                m_textData.Text.push_back(pair<char, shared_ptr<Sprite>>(*text, nullptr));
                ++text;
                continue;
            }
            break;
        case ' ':
            {
                m_curFontDesc.Position.x += (float)m_curFontDesc.Size / (2 * m_parent->GetWidth());
                m_textData.Text.push_back(pair<char, shared_ptr<Sprite>>(*text, nullptr));
                ++text;
                continue;
            }
            break;
        }

        const GlyphDesc* glyphDesc = getGlyphDesc(*text);
        if (glyphDesc) {
            float	 width_n = (float)glyphDesc->Width / m_parent->GetWidth();;
            float	 height_n = (float)glyphDesc->Height / m_parent->GetHeight();
            // Render glyph texture to font texture
            Rect texRect;
            texRect.Left = 0;
            texRect.Right = 1.0f;
            texRect.Top = 0;
            texRect.Bottom = 1.0f;
            shared_ptr<Sprite> glyph = std::make_shared<Sprite>(m_renderContext);
            glyph->SetSize(width_n, height_n);
            glyph->SetTextureCoords(texRect);
            glyph->SetTexture(glyphDesc->Texture);
            glyph->SetPosition(m_curFontDesc.Position.x, m_curFontDesc.Position.y - (float)m_fontFace->glyph->bitmap_top / m_parent->GetHeight());
            m_curFontDesc.Position.x += width_n + (float)m_fontFace->glyph->bitmap_left / m_parent->GetWidth();
            m_textData.Text.push_back(pair<char, shared_ptr<Sprite>>(*text, std::move(glyph)));
        }
        text++;
        if (m_prerenderedText) m_prerenderedText = NULL; // prerendered data is invalid so we clear it
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
    m_prerenderIsDirty = false;
}