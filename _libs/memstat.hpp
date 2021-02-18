//------------------------------------------------------------------------------
// File: memstat.hpp
// Author: Mikael Nilsson
// Contributor(s): Erik Ström
// Version: 0.2
// Orig. Date: 2016-02-11
// Update: 2019-12-29
//------------------------------------------------------------------------------

#pragma once

#include "mempool.hpp"

static MemPool mempool;

void* operator new(std::size_t size) noexcept(false) {
    void* mem = mempool.create(size, false);
    if (mem == nullptr) {
        throw std::bad_alloc();
    }
    return mem;
}

void* operator new[](std::size_t size) noexcept(false) {
    void* mem = mempool.create(size, true);
    if (mem == nullptr) {
        throw std::bad_alloc();
    }
    return mem;
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
    return mempool.create(size, false);
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept {
    return mempool.create(size, true);
}

void operator delete(void* element) throw() {
    mempool.remove(element, false);
}

void operator delete[](void* element) throw() {
    mempool.remove(element, true);
}

void operator delete(void* element, const std::nothrow_t&) throw() {
    mempool.remove(element, false);
}

void operator delete[](void* element, const std::nothrow_t&) throw() {
    mempool.remove(element, true);
}


