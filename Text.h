#pragma once
#include "common.h"
#include "IRenderable.h"
#include "Sprite.h"
#include <d3dx9.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_UNPATENTED_H

class RenderContext;

class Text : public IRenderElement {
    struct Iterator;
    struct set_size;
    struct set_pos;
    struct set_color;
public:
    Text(RenderContext* renderContext, std::string& fontPath, float fontSize, IRenderable* parent);
    ~Text();
    virtual void SetPosition();
    // IRenderElement
    virtual uint32_t GetPixelWidth() const;
    virtual uint32_t GetPixelHeight() const;
    virtual float GetWidth() const;
    virtual float GetHeight() const;
    virtual float GetX() const;
    virtual float GetY() const;
    // Iteration functions
    virtual Iterator Begin() const;
    virtual Iterator End() const;
    virtual Iterator Erase(Iterator& iter);
    // Render functions
    virtual void Clear();
    virtual void Render();
    // Operators
    virtual Text& operator<<(const set_size& token);
    //virtual Text& operator<<(const set_pos&  token);
    virtual Text& operator<<(const set_color& token);
    virtual Text& operator<<(const char* text);
    virtual Text& operator<<(const std::string& text);
public:
    struct set_size {
        set_size(uint32_t size) : m_size(size) {
        }
    private:
        friend Text;
        uint32_t m_size;
    };
    
    //struct set_pos {
    //    set_pos(float x, float y) : m_pos(x, y) {
    //    }
    //private:
    //    friend Text;
    //    Vector2<float> m_pos;
    //};

    struct set_color {
        set_color(float r, float g, float b) : m_color(r, g, b) {
        }
    private:
        friend Text;
        Vector3<float> m_color;
    };
    
    struct FontDesc {
        FontDesc() : Size(0.0f), Position(0.0f, 0.0f), Color(1.0f, 1.0f, 1.0f) {}
        float           Size;
        Vector2<float>  Position;
        Vector3<float>  Color;
    };

    struct Iterator {
        Iterator(const Iterator& iter) {
            *this = iter;
        }
        Iterator(const Iterator&& iter) {
            *this = std::move(iter);
        }
        Iterator& operator=(const Iterator& iter) {
            m_iter = iter.m_iter;
            return *this;
        }
        Iterator& operator=(const Iterator&& iter) {
            m_iter = std::move(iter.m_iter);
            return *this;
        }
        char operator*() const {
            return m_iter->first;
        }

        const FontDesc& operator@() const {
            return m_iter->second;
        }

        Iterator& operator++() {
            ++m_iter;
            return *this;
        }
        Iterator operator++(int) {
            Iterator i = *this;
            operator++();
            return i;
        }
        Iterator& operator--() {
            --m_iter;
            return *this;
        }
        Iterator operator--(int) {
            Iterator i = *this;
            operator--();
            return i;
        }
        bool operator==(const Iterator& arg) const {
            return m_iter == arg.m_iter;
        }
        bool operator!=(const Iterator& arg) const {
            return m_iter != arg.m_iter;
        }
    private:
        friend Text;
        Iterator();
        Iterator(list<pair<char, FontDesc>>::const_iterator& iter) : m_iter(iter) {};
        list<pair<char, FontDesc>>::const_iterator m_iter;
    };
    
private:
    map<uint8_t, CComPtr<IDirect3DTexture9>> m_glyphTextures;

    IRenderable*    m_parent;
    RenderContext*  m_renderContext;
    FT_Face			m_fontFace;
    FontDesc        m_curFontDesc;

    list<pair<char, FontDesc>>  m_text;
    Rect                        m_boundingRect;
    Sprite                      m_prerenderedText;
    bool                        m_prerenderIsDirty;

    const CComPtr<IDirect3DTexture9>& getGlyphTexture(uint8_t c);
    void processString(const char* text);
    void prerenderText();
};

inline uint32_t Text::GetPixelWidth() const {
    m_prerenderedText.GetPixelWidth();
}

inline uint32_t Text::GetPixelHeight() const {
    m_prerenderedText.GetPixelHeight();
}

inline float Text::GetWidth() const {
    m_prerenderedText.GetWidth();
}

inline float Text::GetHeight() const {
    m_prerenderedText.GetHeight();
}

inline float Text::GetX() const {
    m_prerenderedText.GetX();
}

inline float Text::GetY() const {
    m_prerenderedText.GetY();
}