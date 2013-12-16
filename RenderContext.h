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
    virtual uint32_t GetWidth() const;
    virtual uint32_t GetHeight() const;
    virtual int32_t GetX() const;
    virtual int32_t GetY() const;
    virtual CComPtr<IDirect3DTexture9> GetTexture();
private:
    friend class RenderContext;
    uint32_t                    m_width;
    uint32_t                    m_height;
    CComPtr<IDirect3DTexture9>  m_texture;
    CComPtr<IDirect3DSurface9>  m_surface;
    RenderContext*              m_renderContext;

    RenderTarget() {}
    RenderTarget(CComPtr<IDirect3DSurface9> rtSurface);
    RenderTarget(RenderContext* renderContext, uint32_t width, uint32_t height, D3DFORMAT format);
    IDirect3DSurface9** operator&();
};

class RenderContext {
    friend class RenderTarget;
public:
	RenderContext(const RenderContextSetup& setup);
	~RenderContext();
    // Getters
    HWND                        GetWindow() const;
    CComPtr<IDirect3D9>	        GetD3D() const;
    CComPtr<IDirect3DDevice9>   GetDevice() const;
    RenderTarget&               GetDefaultRT();
    RenderTarget&               GetCurrentRT();
	
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
	HWND						m_window;
	CComPtr<IDirect3D9>			m_d3d;
	CComPtr<IDirect3DDevice9>	m_device;
    RenderTarget                m_defaultRT;
    RenderTarget                m_currentRT;

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

/// RenderTarget
inline uint32_t RenderTarget::GetWidth() const {
    return m_width;
}

inline uint32_t RenderTarget::GetHeight() const {
    return m_height;
}

inline int32_t RenderTarget::GetX() const {
    return 0;
}

inline int32_t RenderTarget::GetY() const {
    return 0;
}

inline CComPtr<IDirect3DTexture9> RenderTarget::GetTexture() {
    return m_texture;
}

/// RenderContext
inline HWND RenderContext::GetWindow() const {
    return m_window;
}

inline CComPtr<IDirect3D9> RenderContext::GetD3D() const {
    return m_d3d;
}

inline CComPtr<IDirect3DDevice9> RenderContext::GetDevice() const {
    return m_device;
}

inline RenderTarget& RenderContext::GetDefaultRT() {
    return m_defaultRT;
}

inline RenderTarget& RenderContext::GetCurrentRT() {
    return m_currentRT;
}