#include "Context.h"

#include "CodeLoader.h"
#include "ModLoader.h"
#include "Patches.h"

void Context::init()
{
    if (!Config::init())
        return;

    if (Config::enableDebugConsole)
    {
        if (!GetConsoleWindow())
            AllocConsole();

        freopen("CONOUT$", "w", stdout);
    }

    Patches::init();
    ModLoader::init();
    CodeLoader::init();
}
