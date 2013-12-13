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

uint32_t LayoutManager::GetWidth() const {
    return m_renderContext->GetWindowWidth();
}

uint32_t LayoutManager::GetHeight() const {
    return m_renderContext->GetWindowHeight();
}

void LayoutManager::Render() {
    for (auto item : m_childs) {
        item->Render();
    }
}