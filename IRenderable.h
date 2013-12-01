#pragma once
#include "common.h"

struct IRenderable {
	virtual void Render() = 0;
};