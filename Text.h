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
public:
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
        Iterator(std::list<std::pair<char, shared_ptr<Sprite>>>::iterator& iter) : m_iter(iter) {};
        std::list<std::pair<char, shared_ptr<Sprite>>>::iterator m_iter;
    };
    
public:
    Text(RenderContext* renderContext, std::string& fontPath, IRenderable* parent);
    ~Text();
    virtual const Vector2<float>& GetCurrentPosition() const;
    virtual const Vector3<float>& GetCurrentColor() const;
    // Iteration functions
    virtual Iterator Begin() const;
    virtual Iterator End() const;
    virtual Iterator Erase(Iterator& iter);
    // Render functions
    virtual void Clear();
    virtual void Render();
    // Operators
    virtual Text& operator<<(const char* text);
    virtual Text& operator<<(const std::string& text);

private:
    struct FontDesc {
        FontDesc() : Size(12), Position(0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f) {}
        uint32_t        Size;
        Vector2<float>  Position;
        Vector3<float>  Color;
    };
    struct GlyphDesc {
        GlyphDesc() : Width(0), Height(0) {}
        uint32_t                    Width;
        uint32_t                    Height;
        CComPtr<IDirect3DTexture9>  Texture;
    };
    struct TextData {
        list<pair<char, shared_ptr<Sprite>>> Text;
        Rect                                 BoundingRect;
    };

    typedef std::map<uint8_t, GlyphDesc> GlyphDescs;

    IRenderable*    m_parent;
    RenderContext*  m_renderContext;
    FT_Face			m_fontFace;
    FontDesc        m_curFontDesc;
    TextData        m_textData;
    GlyphDescs      m_glyphDescs;

    CComPtr<IDirect3DTexture9>   m_prerenderedText;

    const GlyphDesc* getGlyphDesc(uint8_t c);
    void processString(const char* text);
    void prerenderText();
};