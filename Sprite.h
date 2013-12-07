#pragma once
#include "common.h"
#include "d3dx9shader.h"
#include "IRenderable.h"

class RenderContext;

struct Rect {
    float Left;
    float Right;
    float Top;
    float Bottom;
};

enum RenderMode {
    RM_SOLID,
    RM_ALPHA_TRANSPARENCY,
    RM_ALPHA_TO_COLOR
};

class Sprite : public IRenderable {
public:
    struct Desc {
        float PosX;
        float PosY;
        float Width;
        float Height;
        Rect  TexCoords;
    };
public:
    Sprite(RenderContext* renderContext, RenderMode mode = RM_ALPHA_TRANSPARENCY);
    virtual ~Sprite();

    virtual void SetTexture(const std::string& texturePath);
    virtual void SetTexture(CComPtr<IDirect3DTexture9> texture);
    virtual void SetPosition(float x, float y);
    virtual void SetSize(float width, float height);
    virtual void SetTextureCoords(Rect coords);

    const Desc& GetDesc();

    virtual void Show(bool show);
    virtual void Render();

private:
    struct Vertex {
        float X, Y, Z, W;
        float U, V;
    };

    Desc                                    m_desc;
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

    ID3DXBuffer* compileShader(const std::string& shaderSource, const std::string& entryPoint, const std::string& profile, ID3DXConstantTable** outConstantTable = NULL) const;
    void copy(const Sprite& s);
};
