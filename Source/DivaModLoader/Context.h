#pragma once

#include "Config.h"

#define LOG(x, ...) \
    { \
        if (Config::enableDebugConsole) \
        { \
            printf(""##x##"\n", __VA_ARGS__); \
        } \
    }

class Context
{
public:
    static void preInit();
    static void init();
    static void postInit();
};