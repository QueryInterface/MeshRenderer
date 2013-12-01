#pragma once
#include "common.h"
#include "IRenderable.h"

class LayoutManager {
public:
	LayoutManager();
	virtual ~LayoutManager();
	virtual void Render();
private:
	std::list<IRenderable> m_childs;
};

