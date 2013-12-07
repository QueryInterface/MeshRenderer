#pragma once
#include "common.h"
#include "IRenderable.h"
#include "Sprite.h"
#include <d3dx9.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_UNPATENTED_H

class RenderContext;

class Font : public IRenderable {
public:
    struct set_size {
        set_size(uint32_t size) : m_size(size) {
        }
    private:
        friend Font;
        uint32_t m_size;
    };

    struct set_pos {
        set_pos(float x, float y) : m_pos(x, y) {
        }
    private:
        friend Font;
        D3DXVECTOR2 m_pos;
    };
    typedef std::string::iterator Iterator;
public:
    Font(RenderContext* renderContext, std::string& fontPath);
    ~Font();
    virtual void Clear();
    virtual void Render();
    virtual Iterator& Begin();
    virtual Iterator& End();
    virtual Iterator& Erase(Iterator& iter);

    virtual Font& operator<<(const set_size& token);
    virtual Font& operator<<(const set_pos&  token);
    virtual Font& operator<<(const char* text);
    virtual Font& operator<<(const std::string& text);

private:
    struct TextBlock {
        std::string	                          Text;	
        std::list<Sprite>                     Glyphs;
        CComPtr<IDirect3DTexture9>            PrerenderedText;
        Rect                                  BoundingRect;
        std::map<const char*, std::list<Sprite>::iterator> CharToGlyph;
    };

    RenderContext*  m_renderContext;
    FT_Library		m_fontLibrary;
    FT_Face			m_fontFace;
    uint32_t		m_fontSize;
    D3DXVECTOR2		m_position;
    CComPtr<IDirect3DTexture9> Texture;

    TextBlock       m_text;

    void setFontSize(uint32_t size);
    void processString(const char* text);
    void prerenderText();
};