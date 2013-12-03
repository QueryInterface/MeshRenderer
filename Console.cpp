#include "Console.h"


Console::Console(RenderContext* renderContext) 
	: Sprite(renderContext) 
	, m_isShown(false) 
	, m_position(0.0f, -0.4f)
	, m_size(1.0f, 0.4f) {
	renderContext->EventSubscribe_KeyPress(this);

	SetSize(m_size.x, m_size.y);
	SetPosition(m_position.x, m_position.y);
}

Console::~Console() {
}

void Console::SetConsoleHeight(float percent) {
	m_size.y = percent;
	SetSize(m_size.x, m_size.y);
}

void Console::Render() {
	if (m_isShown && m_position.y < 0.0f) {
		m_position.y += 0.05f;
		SetPosition(m_position.x, m_position.y);
	}
	if (!m_isShown && m_position.y + m_size.y > 0.0f) {
		m_position.y-= 0.05f;
		SetPosition(m_position.x, m_position.y);
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

