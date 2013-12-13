#pragma once
#include "common.h"

enum RENDER_PROPERTY {
    RP_SCALE
};

struct IRenderable {
    virtual ~IRenderable() = 0;
	virtual void Render() = 0;
    virtual uint32_t GetWidth() const {return 0;};
    virtual uint32_t GetHeight() const {return 0;};
    virtual void SetPropety(RENDER_PROPERTY property, bool value) {};
};