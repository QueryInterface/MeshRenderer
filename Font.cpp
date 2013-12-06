#include "Font.h"
#include "RenderContext.h"
#include "Sprite.h"
#include <direct.h>

#define FONT_TEX_WIDTH  512
#define FONT_TEX_HEIGHT 512

Font::Font(RenderContext* renderContext, std::string& fontPath) 
	: m_renderContext(renderContext)
	, m_fontSize(12)
	, m_position(0.0f, 0.0f) {
	if(FT_Init_FreeType(&m_fontLibrary)) {
		throw std::runtime_error("Failed to create FreeType object");
	}
	if(FT_New_Face(m_fontLibrary, fontPath.c_str(), 0, &m_fontFace)) {
		throw std::runtime_error("Failed to create FreeType font face");
	}
	m_curFontDesc = setFontSize(m_fontSize);
}

Font::~Font() {
}

void Font::Clear() {
	m_text.clear();	
}

void Font::Render() {
	std::for_each(m_textGlyphs.begin(), m_textGlyphs.end(), [](Sprite& s) {s.Render();});
}

inline Font::Iterator& Font::Begin() {
	return m_text.begin();
}

inline Font::Iterator& Font::End() {
	return m_text.end();
}

inline Font::Iterator& Font::Erase(Iterator& iter) {
	// TODO: Optimize so erase is performed with constant time
	Iterator ret = m_text.erase(iter);
	Clear();
	processString(m_text.c_str());
	return ret;
}

Font& Font::operator<<(const set_size& token) {
	m_curFontDesc = setFontSize(token.m_size);
	return *this;
}

Font& Font::operator<<(const set_pos&  token) {
	m_position = token.m_pos;
	return *this;
}

Font& Font::operator<<(const char* text) {
	processString(text);
	return *this;
}

Font& Font::operator<<(const std::string& text) {
	processString(text.c_str());
	return *this;
}

Font::FontDesc& Font::setFontSize(uint32_t size) {
	// Update size
	m_fontSize = size;
	FT_Set_Pixel_Sizes(m_fontFace, 0, m_fontSize);
	// Check if we have texture for that size already
	auto fontTextureIter = m_fontTextures.find(m_fontSize);
	if (fontTextureIter != m_fontTextures.end()) {
		return fontTextureIter->second;
	}
	// Render letters to texture
	FontDesc& fontDesc = m_fontTextures[m_fontSize];
	Sprite letterSprite(m_renderContext);
	CComPtr<IDirect3DTexture9> fontTexture;
	CComPtr<IDirect3DSurface9> fontSurface;

	CHECK(m_renderContext->Device->CreateTexture(FONT_TEX_WIDTH, FONT_TEX_HEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8, D3DPOOL_DEFAULT, &fontTexture, NULL), "Failed to create font texture");
	CHECK(fontTexture->GetSurfaceLevel(0, &fontSurface), "Failed to get font surface level");
	CHECK(m_renderContext->Device->SetRenderTarget(0, fontSurface), "Failed to set render target");
	m_renderContext->Clear();
	uint32_t x = 0, y = 0, row_height = 0;
	for (uint32_t i = 33; i < 128; ++i) {
		if(!FT_Load_Char(m_fontFace, i, FT_LOAD_RENDER)) {
			CComPtr<IDirect3DTexture9> glyphTexture;
			CComPtr<IDirect3DSurface9> glyphSurface;

			uint32_t width = m_fontFace->glyph->bitmap.width;
			uint32_t height = m_fontFace->glyph->bitmap.rows;
			float	 width_n = (float)width / m_renderContext->GetWindowWidth();
			float	 height_n = (float)height / m_renderContext->GetWindowHeight();
			float	 x_n = (float)(x + m_fontFace->glyph->bitmap_left) / FONT_TEX_WIDTH;
			float	 y_n = (float)(y - m_fontFace->glyph->bitmap_top) / FONT_TEX_HEIGHT;
			uint32_t size = width * height;
			x += width;
			// Check if we need to go one row down
			if (x + width > FONT_TEX_WIDTH) {
				x = 0;
				y += row_height;
				if (y + height > FONT_TEX_HEIGHT) {
					// TODO: place warning here
				}
			}
			// Store glyph data
			GlyphDesc& glyphDesc = fontDesc.Glyphs[i];
			glyphDesc.TexX = x_n;
			glyphDesc.TexY = y_n;
			glyphDesc.Width = width_n;
			glyphDesc.Height = height_n;
			glyphDesc.TopShift = (float)m_fontFace->glyph->bitmap_top / FONT_TEX_WIDTH;
			glyphDesc.LeftShift = (float)m_fontFace->glyph->bitmap_left / FONT_TEX_HEIGHT;
			// Create glyph texture
			if (height > row_height) row_height = height;
			CHECK(m_renderContext->Device->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8, D3DPOOL_DEFAULT, &glyphTexture, NULL), "Failed to create glyph texture");
			D3DLOCKED_RECT rect;
			glyphTexture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);
			uint8_t* src = m_fontFace->glyph->bitmap.buffer;
			uint8_t* dst = (uint8_t*)rect.pBits;
			for (uint32_t i = 0; i < height; i++) {
				memcpy(dst, src, width);
				dst += rect.Pitch;
				src += width;
			}
			memcpy(rect.pBits, m_fontFace->glyph->bitmap.buffer, size);
			glyphTexture->UnlockRect(0);
			// Render glyph texture to font texture
			letterSprite.SetSize(width_n, height_n);
			letterSprite.SetPosition(x_n, y_n);
			letterSprite.SetTexture(glyphTexture);
			letterSprite.Render();
		}
	}
	CHECK(m_renderContext->Device->SetRenderTarget(0, m_renderContext->DefaultRT), "Failed to restore render target");
	fontDesc.Texture = fontTexture;
	fontDesc.LineHeight = (float)row_height / FONT_TEX_HEIGHT;
	return fontDesc;
}

void Font::processString(const char* text) {
	while (*text) {
		if (*text == '\n') {
			m_position.y += m_curFontDesc.LineHeight;
		}
		auto glyphDescIter = m_curFontDesc.Glyphs.find(*text);
		if (glyphDescIter == m_curFontDesc.Glyphs.end()) {
			continue;
		}
		GlyphDesc& glyphDesc = glyphDescIter->second;
		// Prepare letter
		Rect texRect;
		texRect.Left = glyphDesc.TexX;
		texRect.Right = glyphDesc.TexX + glyphDesc.Width;
		texRect.Top = glyphDesc.TexY;
		texRect.Bottom = glyphDesc.TexY + glyphDesc.Height;
		Sprite glyph(m_renderContext);
		glyph.SetSize(glyphDesc.Width, glyphDesc.Height);
		glyph.SetTextureCoords(texRect);
		glyph.SetTexture(m_curFontDesc.Texture);
		glyph.SetPosition(m_position.x, m_position.y - glyphDesc.TopShift);

		m_position.x += glyphDesc.Width + glyphDesc.LeftShift;
		m_textGlyphs.push_back(std::move(glyph));
		m_text += *text;	
		m_charToGlyph[(--m_text.end())._Ptr] = --m_textGlyphs.end();
		text++;
	}
}