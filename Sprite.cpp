#include "Sprite.h"
#include "RenderContext.h"
#include <d3dx9.h>

static const std::string g_vertexShaderSource = 
    "struct VS_INPUT {                                      \n\
        float4 Position : POSITION;                         \n\
        float2 TexCoord : TEXCOORD0;                        \n\
    };                                                      \n\
                                                            \n\
    struct VS_OUTPUT {                                      \n\
        float4 Position : POSITION;                         \n\
        float2 TexCoord : TEXCOORD0;                        \n\
    };                                                      \n\
                                                            \n\
    float4x4 g_matrix;                                      \n\
                                                            \n\
    VS_OUTPUT vs_main (VS_INPUT input) {                    \n\
        VS_OUTPUT output;                                   \n\
        output.Position = mul(input.Position, g_matrix);    \n\
        output.TexCoord = input.TexCoord;                   \n\
        return output;                                      \n\
    }                                                       \n";

static const std::string g_pixelShaderSource = 
    "struct PS_INPUT {                                      \n\
        float4 Position : POSITION;                         \n\
        float2 TexCoord : TEXCOORD0;                        \n\
    };                                                      \n\
                                                            \n\
    struct PS_OUTPUT {                                      \n\
        float4 Color : COLOR0;                              \n\
    };                                                      \n\
                                                            \n\
    sampler2D Tex0;                                         \n\
    float4    Color;                                        \n\
                                                            \n\
    PS_OUTPUT ps_main(PS_INPUT input ) {                    \n\
        PS_OUTPUT output;                                   \n\
        float4 texColor = tex2D(Tex0, input.TexCoord);      \n\
        output.Color = mul(Color, texColor);			    \n\
        //output.Color = float4(0.9f, 0.8f, 0.4, 1);        \n\
        return output;                                      \n\
    }                                                       \n";

Sprite::Sprite(RenderContext* renderContext)
    : m_renderContext(renderContext)
    , m_visible(true) {
    memset(&m_vertices, 0, sizeof(m_vertices)); 
    m_vertices[0].Z = m_vertices[1].Z = m_vertices[2].Z = m_vertices[3].Z = 0.5;
    m_vertices[0].W = m_vertices[1].W = m_vertices[2].W = m_vertices[3].W= 1.0;
    m_vertices[0].U = 0.0f;
    m_vertices[0].V = 0.0f;
    m_vertices[1].U = 1.0f;
    m_vertices[1].V = 0.0f;
    m_vertices[2].U = 0.0f;
    m_vertices[2].V = 1.0f;
    m_vertices[3].U = 1.0f;
    m_vertices[3].V = 1.0f;

    m_vertices[0].X = 0.0f;
    m_vertices[0].Y = 0.0f;
    m_vertices[1].X = 1.0f;
    m_vertices[1].Y = 0.0f;
    m_vertices[2].X = 0.0f;
    m_vertices[2].Y = -1.0f;
    m_vertices[3].X = 1.0f;
    m_vertices[3].Y = -1.0f;
    // Create vertex declaration
    D3DVERTEXELEMENT9 elements[] = {
        {0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0},
        D3DDECL_END()
    };
    CHECK(m_renderContext->Device->CreateVertexDeclaration(elements, &m_vertexDeclaration), "Failed to create vertex declaration");
    // Create vertex buffer
    CHECK(m_renderContext->Device->CreateVertexBuffer(sizeof(m_vertices), 0, 0, D3DPOOL_DEFAULT, &m_vertexBuffer, NULL), "Failed to create vertex buffer");
    void* pData = NULL;
    CHECK(m_vertexBuffer->Lock(0, sizeof(m_vertices), &pData, 0), "Failed to lock vertex bufffer");
    memcpy(pData, m_vertices, sizeof(m_vertices));
    CHECK(m_vertexBuffer->Unlock(), "Failed to unlock vertex buffer");
    // Create shaders
    CComPtr<ID3DXBuffer> shaderBinary;
    shaderBinary = compileShader(g_vertexShaderSource, "vs_main", "vs_2_0", &m_vsConstantTable);
    CHECK(m_renderContext->Device->CreateVertexShader((DWORD*)shaderBinary->GetBufferPointer(), &m_vertexShader), "Failed to create vertex shader");
    shaderBinary = compileShader(g_pixelShaderSource, "ps_main", "ps_2_0", &m_psConstantTable);
    CHECK(m_renderContext->Device->CreatePixelShader((DWORD*)shaderBinary->GetBufferPointer(), &m_pixelShader), "Failed to create pixel shader");
    // Set transformation matrix to identity
    D3DXMatrixIdentity(&m_scaleMatrix);
    D3DXMatrixIdentity(&m_translateMatrix);
    D3DXMatrixIdentity(&m_resultMatrix);
}

Sprite::~Sprite() {

}

void Sprite::SetTexture(const std::string& texturePath ) {
    D3DXCreateTextureFromFile(m_renderContext->Device, texturePath.c_str(), &m_texture);
}

void Sprite::SetTexture(CComPtr<IDirect3DTexture9> texture) {
    m_texture = texture;
}

void Sprite::SetPosition(float x, float y) {
    float x_shift = -1.0f + 2.0f * x;
    float y_shift = 1.0f - 2.0f * y;
    D3DXMatrixTranslation(&m_translateMatrix, x_shift, y_shift, 0.0f);
    D3DXMatrixMultiply(&m_resultMatrix, &m_scaleMatrix, &m_translateMatrix);
    CHECK(m_vsConstantTable->SetMatrix(m_renderContext->Device, "g_matrix", &m_resultMatrix), "Failed to set g_matrix variable in VS shader");
    m_desc.PosX = x;
    m_desc.PosY = y;
}

void Sprite::SetSize(float width, float height) {
    if (width < 0.0f || height  < 0.0f) {
        throw std::runtime_error("Invalid size values");
    }
    float w = 2.0f * width;
    float h = 2.0f * height;
    D3DXMatrixScaling(&m_scaleMatrix, w, h, 1.0f);
    D3DXMatrixMultiply(&m_resultMatrix, &m_scaleMatrix, &m_translateMatrix);
    CHECK(m_vsConstantTable->SetMatrix(m_renderContext->Device, "g_matrix", &m_resultMatrix), "Failed to set g_matrix variable in VS shader");
    m_desc.Width = width;
    m_desc.Height = height;
}

void Sprite::SetColor(float r, float g, float b) {
    m_color.x = r;
    m_color.y = g;
    m_color.z = b;
    m_color.w = 1.0f;
}

void Sprite::SetTextureCoords(Rect coords) {
    m_vertices[0].U = coords.Left;
    m_vertices[0].V = coords.Top;
    m_vertices[1].U = coords.Right;
    m_vertices[1].V = coords.Top;
    m_vertices[2].U = coords.Left;
    m_vertices[2].V = coords.Bottom;
    m_vertices[3].U = coords.Right;
    m_vertices[3].V = coords.Bottom;

    void* pData = NULL;
    CHECK(m_vertexBuffer->Lock(0, sizeof(m_vertices), &pData, 0), "Failed to lock vertex bufffer");
    memcpy(pData, m_vertices, sizeof(m_vertices));
    CHECK(m_vertexBuffer->Unlock(), "Failed to unlock vertex buffer");
    m_desc.TexCoords = coords;
}

void Sprite::Render() {
    if (!m_visible) return;
    CComPtr<IDirect3DDevice9> device = m_renderContext->Device;
    device->BeginScene();
    m_vsConstantTable->SetMatrix(m_renderContext->Device, "g_matrix", &m_resultMatrix);
    m_psConstantTable->SetFloatArray(m_renderContext->Device, "Color", (float*)&m_color, 4);
    device->SetVertexShader(m_vertexShader);
    device->SetPixelShader(m_pixelShader);
    device->SetTexture(0, m_texture);
    device->SetVertexDeclaration(m_vertexDeclaration);
    device->SetStreamSource(0, m_vertexBuffer, 0, sizeof(Vertex));
    device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    device->EndScene();
}

uint32_t Sprite::GetWidth() const {
    return m_desc.Width;
}

uint32_t Sprite::GetHeight() const {
    return m_desc.Height;
}

const Rect& Sprite::GetTexCoords() const {
    return m_desc.TexCoords;
}

float Sprite::GetPosX() {
    return m_desc.PosX;
}

float Sprite::GetPosY() {
    return m_desc.PosY;
}

inline void Sprite::Show(bool show) {
    m_visible = show;
}

ID3DXBuffer* Sprite::compileShader(const std::string& shaderSource, const std::string& entryPoint, const std::string& profile, ID3DXConstantTable** outConstantTable) const {
    ID3DXBuffer* binaryShader = NULL;
    CComPtr<ID3DXBuffer> shaderErrors = NULL;
    DWORD flags = 0;
#ifdef _DEBUG
    flags = D3DXSHADER_DEBUG;
#endif
    CHECK(D3DXCompileShader(shaderSource.c_str(), shaderSource.length(), NULL, NULL, entryPoint.c_str(), profile.c_str(), flags, &binaryShader, &shaderErrors, outConstantTable),
        (char*)shaderErrors->GetBufferPointer());
    return binaryShader;
}