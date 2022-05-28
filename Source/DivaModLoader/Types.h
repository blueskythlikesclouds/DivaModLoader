#pragma once

#include "Allocator.h"

// Using std types allocated within the game require us to manipulate
// them with the same allocator, otherwise we'll run into crashes.
// The types have been put under the "prj" namespace for differentiation.
namespace prj
{
    using string = std::basic_string<char, std::char_traits<char>, Allocator<char>>;

    template<typename T>
    using vector = std::vector<T, Allocator<T>>;
}
