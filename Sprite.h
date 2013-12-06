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

class Sprite : public IRenderable {
public:
	Sprite(RenderContext* renderContext);
	virtual ~Sprite();

	virtual void SetTexture(const std::string& texturePath);
	virtual void SetTexture(CComPtr<IDirect3DTexture9> texture);
	virtual void SetPosition(float x, float y);
	virtual void SetSize(float width, float height);
	virtual void SetTextureCoords(Rect coords);
	virtual void Show(bool show);

	virtual void Render();

private:
	struct Vertex {
		float X, Y, Z, W;
		float U, V;
	};

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
