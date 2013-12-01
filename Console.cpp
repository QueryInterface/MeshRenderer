#include "Console.h"


Console::Console(RenderContext* renderContext) 
	: Sprite(renderContext) 
	, m_isShown(false) 
	, m_height(0.4f) {
	renderContext->EventSubscribe_KeyPress(this);

	m_consoleRect.Left = 0.0f;
	m_consoleRect.Right = 1.0f;
	m_consoleRect.Top = -m_height;
	m_consoleRect.Bottom = 0.0f;
	SetCoords(m_consoleRect);
}

Console::~Console() {
}

void Console::SetConsoleHeight(float percent) {
	m_height = percent;
	m_consoleRect.Bottom = m_consoleRect.Top + m_height;
}

void Console::Render() {
	if (m_isShown && m_consoleRect.Bottom < m_height) {
		m_consoleRect.Bottom += 0.05f;
		m_consoleRect.Top += 0.05f;
		SetCoords(m_consoleRect);
	}
	if (!m_isShown && m_consoleRect.Bottom > 0) {
		m_consoleRect.Bottom -= 0.05f;
		m_consoleRect.Top -= 0.05f;
		SetCoords(m_consoleRect);
	}
	Sprite::Render();
}

void Console::Show(bool show) {
	m_isShown = show;
}

void Console::ProcessKey(uint32_t keyCode, bool isPressed) {
	char ckey = MapVirtualKey(keyCode, MAPVK_VK_TO_CHAR);
	switch (keyCode) {
	case 0xc0:
	case 0xdc:
		{
			if (isPressed) {
				Show(!m_isShown);
			}
		}
		break;
	}
}

void Console::ProcessMouse() {
}

