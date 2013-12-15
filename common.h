#pragma once

#define D3D_DEBUG_INFO

#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>	
#include <stdexcept>
#include <stdint.h>
#include <atlbase.h>
#include <d3d9.h>
#include <memory>
#include <string>
#include "MathBasics.h"

using std::unique_ptr;
using std::shared_ptr;
using std::list;
using std::pair;

#define COPY_GUARD(ClassName) ClassName(const ClassName&); ClassName& operator=(const ClassName&);
#define CHECK(x, message) if (FAILED(x)) {throw std::runtime_error(message);}