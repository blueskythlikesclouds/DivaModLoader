#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <detours.h>

#include <d3d11.h>

#include <cstdint>
#include <cstdio>

// This is a trick to make the byte sizes of these types the same as the game.
#if defined(_STRING_) || defined(_VECTOR_)
#error "String or vector have been already included"
#endif

#pragma push_macro("_ITERATOR_DEBUG_LEVEL")
#undef _ITERATOR_DEBUG_LEVEL
#define _ITERATOR_DEBUG_LEVEL 0

#include <string>
#include <vector>
#include <list>

#undef _ITERATOR_DEBUG_LEVEL
#pragma pop_macro("_ITERATOR_DEBUG_LEVEL")

#include <filesystem>

#include <Helpers.h>
#include <toml.hpp>