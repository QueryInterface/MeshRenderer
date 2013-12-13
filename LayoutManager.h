#pragma once
#include "common.h"
#include "IRenderable.h"

class RenderContext;

class LayoutManager : public IRenderable {
public:
	LayoutManager(RenderContext* renderContext);
	virtual ~LayoutManager();
    virtual void AddItem(IRenderable* item, const std::string& tag);
    virtual uint32_t GetWidth() const;
    virtual uint32_t GetHeight() const;
    virtual void Render();
private:
    RenderContext*  m_renderContext;
	std::list<IRenderable*> m_childs;
};

