#pragma once
#include "common.h"

enum RENDER_PROPERTY {
    RP_SCALE
};

struct IRenderable {
    virtual void Render() = 0;
};

struct IRenderDesc {
    virtual uint32_t GetPixelWidth() const = 0;
    virtual uint32_t GetPixelHeight() const = 0;
    virtual float GetWidth() const = 0;
    virtual float GetHeight() const = 0;
    virtual float GetX() const = 0;
    virtual float GetY() const = 0;
};

struct IRenderElement 
    : public IRenderable
    , public IRenderDesc {
};