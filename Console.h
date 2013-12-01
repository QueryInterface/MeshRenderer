#pragma once
#include "common.h"
#include "Sprite.h"
#include "RenderContext.h"

class RenderContext;

class Console 
	: public Sprite 
	, public IKeyPressCallback 
	, public IMouseCallback {
public:
	Console(RenderContext* renderContext);
	virtual ~Console();
	virtual void SetConsoleHeight(float percent);
	virtual void Render();
	virtual void Show(bool show);
	virtual void ProcessKey(uint32_t keyCode, bool isPressed);
	virtual void ProcessMouse();

private:
	Rect m_consoleRect;
	float m_height;
	bool m_isShown;
};