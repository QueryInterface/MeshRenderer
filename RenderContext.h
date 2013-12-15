#pragma once
#include "common.h"
#include "IRenderable.h"

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

class RenderTarget : public IRenderDesc {
public:
    virtual void MakeCurrent();
    virtual uint32_t GetPixelWidth() const;
    virtual uint32_t GetPixelHeight() const;
    virtual float GetWidth() const;
    virtual float GetHeight() const;
    virtual float GetX() const;
    virtual float GetY() const;
private:
    friend class RenderContext;
    uint32_t                   m_width;
    uint32_t                   m_height;
    CComPtr<IDirect3DSurface9> m_surface;
    RenderContext* m_renderContext;

    RenderTarget() {}
    RenderTarget(CComPtr<IDirect3DSurface9> rtSurface);
    RenderTarget(RenderContext* renderContext, uint32_t width, uint32_t height, D3DFORMAT format);
    IDirect3DSurface9** operator&();
};

class RenderContext {
public:
	HWND						Window;
	CComPtr<IDirect3D9>			D3D;
	CComPtr<IDirect3DDevice9>	Device;
    RenderTarget                DefaultRT;

	RenderContext(const RenderContextSetup& setup);
	~RenderContext();
	
    RenderTarget CreateRenderTarget(uint32_t width, uint32_t height, D3DFORMAT format);

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

inline uint32_t RenderTarget::GetPixelWidth() const {
    return m_width;
}

inline uint32_t RenderTarget::GetPixelHeight() const {
    return m_height;
}

inline float RenderTarget::GetWidth() const {
    return 1.0f;
}

inline float RenderTarget::GetHeight() const {
    return 1.0f;
}

inline float RenderTarget::GetX() const {
    return 0.0f;
}

inline float RenderTarget::GetY() const {
    return 0.0f;
}