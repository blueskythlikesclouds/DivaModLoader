#include "Context.h"

#include "CodeLoader.h"
#include "ModLoader.h"
#include "Patches.h"
#include "SigScan.h"

void Context::init()
{
    if (!sigValid)
    {
        MessageBoxW(nullptr, L"Failed to install mod loader (game version is possibly unsupported)", L"DIVA Mod Loader", MB_ICONERROR);
        return;
    }

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
