#include "RenderContext.h"

RenderContext::RenderContext(const RenderContextSetup& setup) 
    : m_width(setup.Width)
    , m_height(setup.Height) {
    initWindow();
    initD3D();
}

RenderContext::~RenderContext() {
}

void RenderContext::Clear() {
	if (Device)
		Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(255, 0, 0, 255), 1.0, 0);
}

void RenderContext::Present() {
	if (Device)
		Device->Present(NULL, NULL, NULL, NULL);
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
    Window = CreateWindow("MeshRenderer", "MeshRenderer",
                              WS_OVERLAPPEDWINDOW, 0, 0, m_width, m_height,
                              NULL, NULL, wc.hInstance, (void*)this);
    if (!Window) {
        throw std::runtime_error("Failed to create window");
    }
	SetWindowPos(Window,0,0,0,m_width, m_height, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
	ShowWindow(Window, true);
}

void RenderContext::initD3D() {
    D3D = Direct3DCreate9(D3D_SDK_VERSION);
    if(!D3D) {
		std::runtime_error("Failed to create Direct3D");
	}

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;

    CHECK(D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
                            &d3dpp, &Device), "Failed to create device");
	CHECK(Device->GetRenderTarget(0, &DefaultRT), "Failed to save default RT");
	Device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
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