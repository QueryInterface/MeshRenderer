#pragma once
#include "common.h"
#include "IRenderable.h"

class RenderContext;
class FT_Library;

class Font : public IRenderable {
public:
	Font(RenderContext* renderContext);
	~Font();


private:
	RenderContext*	m_renderContext;
	FT_Library*		m_fontLibrary;
};