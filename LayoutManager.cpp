#include "LayoutManager.h"
#include "RenderContext.h"

LayoutManager::LayoutManager(RenderContext* renderContext) 
    : m_renderContext(renderContext) {
}

LayoutManager::~LayoutManager() {
}

void LayoutManager::AddItem(IRenderable* item, const std::string& tag) {
    m_childs.push_back(item);
}

void LayoutManager::Render() {
    for (auto item : m_childs) {
        item->Render();
    }
}