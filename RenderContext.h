#pragma once
#include "common.h"

struct RenderContextSetup {
	uint32_t Width;
	uint32_t Height;
};

struct IKeyPressCallback {
	virtual void ProcessKey(uint32_t keyCode, bool isPress) = 0;
};
struct IMouseCallback {
	virtual void ProcessMouse() = 0;
};

class RenderContext {
public:
	HWND						Window;
	CComPtr<IDirect3D9>			D3D;
	CComPtr<IDirect3DDevice9>	Device;
	CComPtr<IDirect3DSurface9>  DefaultRT;

	RenderContext(const RenderContextSetup& setup);
	~RenderContext();
	
	CComPtr<IDirect3DTexture9> CreateRenderTarget(uint32_t width, uint32_t height, D3DFORMAT format);
	void SetRenderTarget(const CComPtr<IDirect3DTexture9>& rt);
    void SetRenderTarget(const CComPtr<IDirect3DSurface9>& rt);

	void Clear();
	void Present();
	uint32_t GetWindowWidth() {return m_width;}
	uint32_t GetWindowHeight() {return m_height;}

	void EventSubscribe_KeyPress(IKeyPressCallback* callback);
	void EventSubscribe_MouseClick(IMouseCallback* callback);
	void EventSubscribe_MouseMove(IMouseCallback* callback);
	void EventUnsubscribe(void* callback);
	bool ProcessMessage();

private:
	uint32_t m_width;
	uint32_t m_height;
	std::set<WPARAM> m_pressedKeys;
	std::set<IKeyPressCallback*>	m_callbacksOnKeyPress;
	std::set<IMouseCallback*>		m_callbacksOnMouseClick;
	std::set<IMouseCallback*>		m_callbacksOnMouseMove;

	COPY_GUARD(RenderContext);
	void initWindow();
	void initD3D();
	void onMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI msgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};