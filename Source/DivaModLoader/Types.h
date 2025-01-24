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

    template<typename T>
    using list = std::list<T, Allocator<T>>;

    template<typename K, typename T>
    using map = std::map<K, T, std::less<K>, Allocator<std::pair<const K, T>>>;

    template <class T>
    struct default_delete
    {
        void operator()(T* ptr) const noexcept
        {
            operatorDelete(ptr);
        }
    };

    template <class T>
    struct default_delete<T[]>
    {
        void operator()(T* ptr) const noexcept
        {
            static_assert(std::is_trivially_destructible_v<T>); // Needs to be trivially destructible for now
            operatorDelete(ptr);
        }
    };

    template<typename T>
    using unique_ptr = std::unique_ptr<T, default_delete<T>>;
}
