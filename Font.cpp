#include "Font.h"
#include "RenderContext.h"
#include "Utils/Utils.h"
#include <direct.h>

Font::Font(RenderContext* renderContext) 
	: m_renderContext(renderContext)
	, m_fontSize(18)
	, m_position(0.0f, 0.0f)
	, m_sprite(m_renderContext) {
	std::string mediaPath = UTILS::GetMediaFolder();
	if(FT_Init_FreeType(&m_fontLibrary)) {
		throw std::runtime_error("Failed to create FreeType object");
	}
	std::string fontPath = mediaPath + "Fonts/Calibri.ttf";
	if(FT_New_Face(m_fontLibrary, fontPath.c_str(), 0, &m_fontFace)) {
		throw std::runtime_error("Failed to create FreeType font face");
	}
	FT_Set_Pixel_Sizes(m_fontFace, 0, m_fontSize);
	FT_Load_Char(m_fontFace, 'X', FT_LOAD_RENDER);
	m_sprite.SetSize(0.1f, 0.1f);
	m_sprite.SetPosition(0.0f, 0.0f);
	CComPtr<IDirect3DTexture9> texture;
	uint32_t glyphSize = m_fontFace->glyph->bitmap.width * m_fontFace->glyph->bitmap.rows;
	//m_renderContext->Device->CreateTexture(m_fontFace->glyph->bitmap.width, m_fontFace->glyph->bitmap.rows, 1, 
	texture;
}

Font::~Font() {
}

void Font::Clear() {
	m_text.clear();	
}

void Font::Render() {

}

Font& Font::operator<<(const set_size& token) {
	m_fontSize = token.m_size;
	return *this;
}

Font& Font::operator<<(const set_pos&  token) {
	m_position = token.m_pos;
	return *this;
}

Font& Font::operator<<(const char* text) {
	m_text += text;
	return *this;
}

Font& Font::operator<<(const std::string& text) {
	m_text += text;
	return *this;
}