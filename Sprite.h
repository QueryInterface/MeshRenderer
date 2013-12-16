#pragma once
#include "common.h"
#include "d3dx9shader.h"
#include "IRenderable.h"
#include "RenderContext.h"

class RenderContext;

struct Rect {
    float Left;
    float Right;
    float Top;
    float Bottom;
};

class Sprite : public IRenderElement {
public:
    Sprite(RenderContext* renderContext);
    virtual ~Sprite();

    virtual void SetTexture(const std::string& texturePath);
    virtual void SetTexture(const CComPtr<IDirect3DTexture9>& texture);
    virtual void SetPosition(int32_t x, int32_t y);
    virtual void SetSize(uint32_t width, uint32_t height);
    virtual void SetColor(float r, float g, float b);
    virtual void SetOpacity(float alpha);
    virtual void SetTextureCoords(Rect coords);

    virtual uint32_t GetWidth() const;
    virtual uint32_t GetHeight() const;
    virtual int32_t GetX() const;
    virtual int32_t GetY() const;
    virtual const CComPtr<IDirect3DTexture9>& GetTexture() const;
    virtual const Rect& GetTexCoords() const;
    virtual Vector3<float> GetColor() const;
    virtual float GetOpacity() const;

    virtual void Show(bool show);
    virtual void Render();

private:
    struct Desc {
        int32_t PosX;
        int32_t PosY;
        uint32_t Width;
        uint32_t Height;
        Rect  TexCoords;
    };
    struct Vertex {
        float X, Y, Z, W;
        float U, V;
    };

    Desc                                    m_desc;
    RenderContext*							m_renderContext;
    bool									m_visible;
    uint32_t                                m_adjustedForWidth;
    uint32_t                                m_adjustedForHeight;

    CComPtr<IDirect3DTexture9>				m_texture;
    CComPtr<IDirect3DVertexDeclaration9>	m_vertexDeclaration;
    CComPtr<IDirect3DVertexBuffer9>			m_vertexBuffer;
    CComPtr<IDirect3DVertexShader9>         m_vertexShader;
    CComPtr<IDirect3DPixelShader9>          m_pixelShader;
    CComPtr<ID3DXConstantTable>             m_vsConstantTable;
    CComPtr<ID3DXConstantTable>             m_psConstantTable;

    Vertex									m_vertices[4];
    D3DXMATRIX                              m_scaleMatrix;
    D3DXMATRIX                              m_translateMatrix;
    D3DXMATRIX								m_resultMatrix;
    D3DXVECTOR4                             m_color;

    ID3DXBuffer* compileShader(const std::string& shaderSource, const std::string& entryPoint, const std::string& profile, ID3DXConstantTable** outConstantTable = NULL) const;
    void copy(const Sprite& s);
    void adjustDimesions();
};

inline uint32_t Sprite::GetWidth() const {
    return m_desc.Width;
}

inline uint32_t Sprite::GetHeight() const {
    return m_desc.Height;
}

inline int32_t Sprite::GetX() const {
    return m_desc.PosX;
}

inline int32_t Sprite::GetY() const {
    return m_desc.PosY;
}

inline const Rect& Sprite::GetTexCoords() const {
    return m_desc.TexCoords;
}

inline const CComPtr<IDirect3DTexture9>& Sprite::GetTexture() const {
    return m_texture;
}

inline Vector3<float> Sprite::GetColor() const {
    return Vector3<float>(m_color.x, m_color.y, m_color.z);
}

inline float Sprite::GetOpacity() const {
    return m_color.w;
}