#pragma once
#include "common.h"

enum RENDER_PROPERTY {
    RP_SCALE
};

struct IRenderable {
    virtual void Render() = 0;
};

struct IRenderDesc {
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual int32_t GetX() const = 0;
    virtual int32_t GetY() const = 0;
};

struct IRenderElement 
    : public IRenderable
    , public IRenderDesc {
};