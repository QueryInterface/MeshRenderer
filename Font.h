#pragma once
#include "common.h"
#include "IRenderable.h"
#include "Sprite.h"
#include <d3dx9.h>
#include <ft2build.h>
#include FT_FREETYPE_H

class RenderContext;

class Font : public IRenderable {
public:
	struct set_size {
		set_size(uint32_t size) : m_size(size) {
		}
	private:
		friend Font;
		uint32_t m_size;
	};

	struct set_pos {
		set_pos(float x, float y) : m_pos(x, y) {
		}
	private:
		friend Font;
		D3DXVECTOR2 m_pos;
	};
public:
	Font(RenderContext* renderContext);
	~Font();
	virtual void Clear();
	virtual void Render();

	virtual Font& operator<<(const set_size& token);
	virtual Font& operator<<(const set_pos&  token);
	virtual Font& operator<<(const char* text);
	virtual Font& operator<<(const std::string& text);

private:
	RenderContext*  m_renderContext;
	FT_Library		m_fontLibrary;
	FT_Face			m_fontFace;
	uint32_t		m_fontSize;
	std::string		m_text;
	D3DXVECTOR2		m_position;
	Sprite			m_sprite;
};