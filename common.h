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
using std::map;
using std::list;
using std::pair;

#define COPY_GUARD(ClassName) ClassName(const ClassName&); ClassName& operator=(const ClassName&);
#define CHECK(x, message) if (FAILED(x)) {throw std::runtime_error(message);}

template <class IteratorType, class ValueType>
class Iterator {
public:
    Iterator(const Iterator& iter) {
        *this = iter;
    }
    Iterator(const Iterator&& iter) {
        *this = std::move(iter);
    }
    Iterator& operator=(const Iterator& iter) {
        m_iter = iter.m_iter;
        return *this;
    }
    Iterator& operator=(const Iterator&& iter) {
        m_iter = std::move(iter.m_iter);
        return *this;
    }
    ValueType& operator*() const {
        return m_iter->first;
    }

    Iterator& operator++() {
        ++m_iter;
        return *this;
    }
    Iterator operator++(int) {
        Iterator i = *this;
        operator++();
        return i;
    }
    Iterator& operator--() {
        --m_iter;
        return *this;
    }
    Iterator operator--(int) {
        Iterator i = *this;
        operator--();
        return i;
    }
    bool operator==(const Iterator& arg) const {
        return m_iter == arg.m_iter;
    }
    bool operator!=(const Iterator& arg) const {
        return m_iter != arg.m_iter;
    }
    Iterator();
    Iterator(IteratorType& iter) : m_iter(iter) {};
    IteratorType m_iter;
};
