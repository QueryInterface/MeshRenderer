#include "Font.h"
#include "RenderContext.h"
#include <ft2build.h>

Font::Font(RenderContext* renderContext) 
	: m_renderContext(renderContext)
	, m_fontLibrary(NULL) {
}

Font::~Font() {
}