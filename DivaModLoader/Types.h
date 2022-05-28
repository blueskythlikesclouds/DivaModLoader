#pragma once

#include "Allocator.h"

// They are actually not under this namespace, but I think it's good to differentiate them.
namespace prj
{
    using string = std::basic_string<char, std::char_traits<char>, Allocator<char>>;

    template<typename T>
    using vector = std::vector<T, Allocator<T>>;
}
