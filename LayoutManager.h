#pragma once
#include "common.h"
#include "IRenderable.h"
#include "RenderContext.h"

class LayoutManager : public IRenderable {
public:
	LayoutManager(RenderContext* renderContext);
	virtual ~LayoutManager();
    virtual void AddItem(IRenderable* item, const std::string& tag);
    virtual uint32_t GetPixelWidth() const;
    virtual uint32_t GetPixelHeight() const;
    virtual float GetWidth() const;
    virtual float GetHeight() const;
    virtual float GetX() const;
    virtual float GetY() const;
    virtual void Render();
private:
    RenderContext*  m_renderContext;
	std::list<IRenderable*> m_childs;
};

inline uint32_t LayoutManager::GetPixelWidth() const {
    return m_renderContext->GetWindowWidth();
}

inline uint32_t LayoutManager::GetPixelHeight() const {
    return m_renderContext->GetWindowHeight();
}

inline float LayoutManager::GetWidth() const {
    return 1.0f;
}

inline float LayoutManager::GetHeight() const {
    return 1.0f;
}

inline float LayoutManager::GetX() const {
    return 0.0f;
}

inline float LayoutManager::GetY() const {
    return 0.0f;
}

