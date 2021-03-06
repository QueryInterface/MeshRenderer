#include "RenderContext.h"

////////////////////////////////////////
// RenderTarget implementation
RenderTarget::RenderTarget(RenderContext* renderContext, uint32_t width, uint32_t height, D3DFORMAT format) 
    : m_width(width)
    , m_height(height)
    , m_renderContext(renderContext) {
    CHECK(m_renderContext->GetDevice()->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &m_texture, NULL), "Failed to create render target");
	CHECK(m_texture->GetSurfaceLevel(0, &m_surface), "Failed to get font surface level");
}

RenderTarget::RenderTarget(CComPtr<IDirect3DSurface9> surface) {
    D3DSURFACE_DESC desc;
    surface->GetDesc(&desc);
    m_width = desc.Width;
    m_height = desc.Height;
}

IDirect3DSurface9** RenderTarget::operator&() {
    return &m_surface;
}

void RenderTarget::MakeCurrent() {
    CHECK(m_renderContext->GetDevice()->SetRenderTarget(0, m_surface), "Failed to set render target");
    m_renderContext->m_currentRT = *this;
}

////////////////////////////////////////
// RenderContext implementation
RenderContext::RenderContext(const RenderContextSetup& setup) 
    : m_width(setup.Width)
    , m_height(setup.Height) {
    initWindow();
    initD3D();
}

RenderContext::~RenderContext() {
}

RenderTarget RenderContext::CreateRenderTarget(uint32_t width, uint32_t height, D3DFORMAT format) {
    return RenderTarget(this, width, height, format);
}

void RenderContext::Clear() {
	if (m_device)
		m_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(255, 0, 0, 255), 1.0, 0);
}

void RenderContext::Present() {
	if (m_device)
		m_device->Present(NULL, NULL, NULL, NULL);
}

void RenderContext::EventSubscribe_KeyPress(IKeyPressCallback* callback) {
	m_callbacksOnKeyPress.insert(callback);
}

void RenderContext::EventSubscribe_MouseClick(IMouseCallback* callback) {
	m_callbacksOnMouseClick.insert(callback);
}

void RenderContext::EventSubscribe_MouseMove(IMouseCallback* callback) {
	m_callbacksOnMouseMove.insert(callback);
}

void RenderContext::EventUnsubscribe(void* callback) {
	m_callbacksOnKeyPress.erase((IKeyPressCallback*)callback);
	m_callbacksOnMouseClick.erase((IMouseCallback*)callback);
	m_callbacksOnMouseMove.erase((IMouseCallback*)callback);
}

bool RenderContext::ProcessMessage() {
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
    {
		if (msg.message == WM_QUIT) {
			return false;
		}
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return true;
}

void RenderContext::initWindow() {
    WNDCLASSEX wc = {
        sizeof( WNDCLASSEX ), CS_CLASSDC, msgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        "MeshRenderer", NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
    m_window = CreateWindow("MeshRenderer", "MeshRenderer",
                              WS_OVERLAPPEDWINDOW, 0, 0, m_width, m_height,
                              NULL, NULL, wc.hInstance, (void*)this);
    if (!m_window) {
        throw std::runtime_error("Failed to create window");
    }
	SetWindowPos(m_window,0,0,0,m_width, m_height, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
	ShowWindow(m_window, true);
}

void RenderContext::initD3D() {
    m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if(!m_d3d) {
		std::runtime_error("Failed to create Direct3D");
	}

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;

    CHECK(m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
                            &d3dpp, &m_device), "Failed to create device");
    m_defaultRT.m_width = GetWindowWidth();
    m_defaultRT.m_height = GetWindowHeight();
    m_defaultRT.m_renderContext = this;
    CHECK(m_device->GetRenderTarget(0, &m_defaultRT), "Failed to save default RT");
	m_device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
    m_currentRT = m_defaultRT;
}

void RenderContext::onMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_SIZING:
	case WM_SIZE:
		Present();
		break;
	case WM_KEYDOWN:
		{
			bool isPressed = false;
			if (m_pressedKeys.find(wParam) == m_pressedKeys.end()) {
				isPressed = true;
				m_pressedKeys.insert(wParam);
			}
			std::for_each(m_callbacksOnKeyPress.begin(), m_callbacksOnKeyPress.end(), [&](IKeyPressCallback* callback) {
				callback->ProcessKey(wParam, isPressed);
			});
		}
		break;
	case WM_KEYUP:
		{
			m_pressedKeys.erase(wParam);
		}
		break;
	case WM_KILLFOCUS:
	case WM_SETFOCUS:
		{
			m_pressedKeys.clear();
		}
		break;
	case WM_MOUSEMOVE:
		break;
	}
}

LRESULT WINAPI RenderContext::msgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	RenderContext* renderContext = (RenderContext*)GetWindowLong(hWnd, GWL_USERDATA);
	if (renderContext != NULL)
		renderContext->onMessage(hWnd, msg, wParam, lParam);

	switch(msg) {
		case WM_CREATE:
			LPCREATESTRUCT lpcs;
			lpcs = (LPCREATESTRUCT)lParam;
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)lpcs->lpCreateParams);
			break;
		case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}