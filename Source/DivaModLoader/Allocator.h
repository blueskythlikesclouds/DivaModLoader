#pragma once

#include "SigScan.h"

static inline FUNCTION_PTR(void*, __fastcall, operatorNew, sigOperatorNew(), size_t);
static inline FUNCTION_PTR(void*, __fastcall, operatorDelete, sigOperatorDelete(), void*);
static inline FUNCTION_PTR(void*, __fastcall, heapCMallocAllocate, sigHeapCMallocAllocate(), size_t type, size_t size, const char* name);

template <class T>
class Allocator
{
public:
    using value_type = T;

    Allocator() noexcept {}
    template <class U> Allocator(Allocator<U> const&) noexcept {}

    value_type* allocate(std::size_t n)
    {
        return reinterpret_cast<value_type*>(operatorNew(n * sizeof(value_type)));
    }

    void deallocate(value_type* p, std::size_t) noexcept
    {
        operatorDelete(reinterpret_cast<void*>(p));
    }
};

template <class T, class U>
bool operator==(Allocator<T> const&, Allocator<U> const&) noexcept
{
    return true;
}

template <class T, class U>
bool operator!=(Allocator<T> const& x, Allocator<U> const& y) noexcept
{
    return !(x == y);
}