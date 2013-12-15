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
    Sprite(RenderContext* renderContext, IRenderDesc* parent = NULL);
    virtual ~Sprite();

    virtual void SetParent(IRenderDesc* parent);
    virtual void SetTexture(const std::string& texturePath);
    virtual void SetTexture(CComPtr<IDirect3DTexture9> texture);
    virtual void SetPosition(float x, float y);
    virtual void SetSize(float width, float height);
    virtual void SetColor(float r, float g, float b);
    virtual void SetOpacity(float alpha);
    virtual void SetTextureCoords(Rect coords);

    virtual uint32_t GetPixelWidth() const;
    virtual uint32_t GetPixelHeight() const;
    virtual float GetWidth() const;
    virtual float GetHeight() const;
    virtual float GetX() const;
    virtual float GetY() const;
    virtual const Rect& GetTexCoords() const;
    virtual Vector3<float> GetColor() const;
    virtual float GetOpacity() const;
    virtual IRenderDesc* GetParent() const;

    virtual void Show(bool show);
    virtual void Render();

private:
    struct Desc {
        float PosX;
        float PosY;
        float Width;
        float Height;
        Rect  TexCoords;
    };
    struct Vertex {
        float X, Y, Z, W;
        float U, V;
    };

    Desc                                    m_desc;
    IRenderDesc*                            m_parent;
    RenderContext*							m_renderContext;
    bool									m_visible;

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
    void adjustSize();
    void adjustPosition();
};

inline uint32_t Sprite::GetPixelWidth() const {
    if (m_parent) {
        return (uint32_t)(m_parent->GetPixelWidth() * GetWidth());
    }
    else {
        return (uint32_t)(m_renderContext->GetWindowWidth() * GetWidth());
    }
}

inline uint32_t Sprite::GetPixelHeight() const {
    if (m_parent) {
        return (uint32_t)(m_parent->GetPixelHeight() * GetHeight());
    }
    else {
        return (uint32_t)(m_renderContext->GetWindowHeight() * GetHeight());
    }
}

inline float Sprite::GetWidth() const {
    return m_desc.Width;
}

inline float Sprite::GetHeight() const {
    return m_desc.Height;
}

inline float Sprite::GetX() const {
    return m_desc.PosX;
}

inline float Sprite::GetY() const {
    return m_desc.PosY;
}

inline const Rect& Sprite::GetTexCoords() const {
    return m_desc.TexCoords;
}

inline Vector3<float> Sprite::GetColor() const {
    return Vector3<float>(m_color.x, m_color.y, m_color.z);
}

inline float Sprite::GetOpacity() const {
    return m_color.w;
}

inline IRenderDesc* Sprite::GetParent() const {
    return m_parent;
}