#pragma once
#include "common.h"
#include "IRenderable.h"
#include "Sprite.h"
#include <d3dx9.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_UNPATENTED_H

class RenderContext;

class Letter {
public:
    char                    GetLetter() const;
    uint32_t                GetSize() const;
    uint32_t                GetOffsetX() const;
    const Vector3<float>&   GetColor() const;
private:
    friend class Text;
    friend class Line;

    char                    m_letter;
    uint32_t                m_size;
    uint32_t                m_offsetX;
    Vector3<float>          m_color;
    shared_ptr<Sprite>      m_sprite;
    Line*                   m_parentLine;
};

class Line {
    typedef Iterator<list<Letter>::const_iterator, Letter> LetterIterator;
public:
    uint32_t        GetOffsetY() const;
    LetterIterator  Begin() const;
    LetterIterator  End() const;
    void            Erase(const LetterIterator& iterator);
    void            Insert(const LetterIterator& iterator, const Text& text);
    void            Insert(const LetterIterator& iterator, const std::string& text);
    void            Insert(const LetterIterator& Iterator, const char* text);
private:
    friend class Text;
    Line(Text* parent);

    Text*           m_parent;
    uint32_t        m_offsetY;    //< Y offset from the left upper corner
    list<Letter>    m_letters;
};

class Text : public IRenderElement {
    typedef Iterator<list<Line>::const_iterator, Line> LineIterator;
public:
    struct set_size;
    struct set_pos;
    struct set_color;
public:
    Text(RenderContext* renderContext, std::string& fontPath, float fontSize, IRenderable* parent);
    ~Text();
    virtual void SetPosition(uint32_t x, uint32_t y);
    virtual void SetLineInterval(uint32_t interval);
    // IRenderElement
    virtual uint32_t GetWidth() const;
    virtual uint32_t GetHeight() const;
    virtual int32_t GetX() const;
    virtual int32_t GetY() const;
    // Access/modification functions
    LineIterator Begin();
    LineIterator End();
    void Erase(const LineIterator& iterator);
    void Insert(const LineIterator& iterator, const Text& text);
    void Insert(const LineIterator& iterator, const std::string& text);
    void Insert(const LineIterator& Iterator, const char* text);
    // Render functions
    virtual void Clear();
    virtual void Render();
    // Operators
    virtual Text& operator<<(const set_size& token);
    virtual Text& operator<<(const set_color& token);
    virtual Text& operator<<(const char* text);
    virtual Text& operator<<(const std::string& text);

    struct set_size {
        set_size(uint32_t size) : m_size(size) {
        }
    private:
        friend Text;
        uint32_t m_size;
    };

    struct set_color {
        set_color(float r, float g, float b) : m_color(r, g, b) {
        }
    private:
        friend Text;
        Vector3<float> m_color;
    };
    
private:
    struct GlyphDesc {
        CComPtr<IDirect3DTexture9> Texture;
        uint32_t Width;
        uint32_t Height;
        uint32_t Top;
        uint32_t Left;
        uint32_t Advance;
    };

    RenderContext*      m_renderContext;
    FT_Face			    m_fontFace;
    Letter              m_curLetter;
    Line                m_curLine;
    Vector2<int32_t>    m_textPosition;
    uint32_t            m_lineInterval;
    list<Line>          m_text;
    bool                m_textAdjusted;

    Rect                m_boundingRect;
    Sprite              m_prerenderedText;
    bool                m_prerendered;

    bool getGlyphDesc(uint8_t c, GlyphDesc& glyphDesc);
    void processString(const char* text);
    void adjustText();
    void prerenderText();
};

/// Letter
inline char Letter::GetLetter() const {
    return m_letter;
}

inline uint32_t Letter::GetSize() const {
    return m_size;
}

inline uint32_t Letter::GetOffsetX() const {
    return m_offsetX;
}

inline const Vector3<float>& Letter::GetColor() const {
    return m_color;
}

/// Line
inline uint32_t Line::GetOffsetY() const {
    return m_offsetY;
}

inline Line::LetterIterator Line::Begin() const {
    return m_letters.begin();
}

inline Line::LetterIterator Line::End() const {
    return m_letters.end();
}

/// Text
inline void Text::SetPosition(uint32_t x, uint32_t y) {
    m_textPosition.x = x;
    m_textPosition.y = y;
}

inline uint32_t Text::GetWidth() const {
    return m_prerenderedText.GetWidth();
}

inline uint32_t Text::GetHeight() const {
    return m_prerenderedText.GetHeight();
}

inline int32_t Text::GetX() const {
    return m_prerenderedText.GetX();
}

inline int32_t Text::GetY() const {
    return m_prerenderedText.GetY();
}

inline Text::LineIterator Text::Begin() {
    if (!m_textAdjusted) adjustText();
    return m_text.begin();
}

inline Text::LineIterator Text::End() {
    if (!m_textAdjusted) adjustText();
    return m_text.end();
}