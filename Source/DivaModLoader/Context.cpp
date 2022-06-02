#include "Context.h"

#include "CodeLoader.h"
#include "ModLoader.h"
#include "Patches.h"
#include "SigScan.h"

HOOK(int, WINAPI, WinMain, sigWinMain(), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Context::initCore();

    return originalWinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

void Context::initCore()
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

void Context::init()
{
    if (!sigValid)
    {
        MessageBoxW(nullptr, L"Failed to install mod loader (game version is possibly unsupported)", L"DIVA Mod Loader", MB_ICONERROR);
        return;
    }

    INSTALL_HOOK(WinMain);
}
