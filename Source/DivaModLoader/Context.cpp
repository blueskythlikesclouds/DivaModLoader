#include "Context.h"

#include "CodeLoader.h"
#include "DatabaseLoader.h"
#include "FileLoader.h"
#include "ModLoader.h"
#include "Patches.h"
#include "SigScan.h"

#pragma comment(linker, "/EXPORT:DirectInput8Create=C:\\Windows\\System32\\dinput8.DirectInput8Create")

// DllMain:
// - Do the least amount of work possible. We don't want to run into loader locks.

// CRT: 
// - Load mods before MM+'s CRT initializer. This lets DLL mods hook into C/C++ initializers using PreInit function.

// WinMain:
// - Call Init and PostInit functions of DLL mods.
// - Install database loader as we insert into a dynamically initialized std::list.

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
        Context::preInit();

    return TRUE;
}

HOOK(int, WINAPI, CrtMain, sigCrtMain(), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Context::init();

    return originalCrtMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

HOOK(int, WINAPI, WinMain, sigWinMain(), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Context::postInit();

    return originalWinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

void Context::preInit()
{
    if (!sigValid)
    {
        MessageBoxW(nullptr, L"Failed to install mod loader (game version is possibly unsupported)", L"DIVA Mod Loader", MB_ICONERROR);
        return;
    }

    INSTALL_HOOK(CrtMain);
}

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
    FileLoader::init();

    INSTALL_HOOK(WinMain);
}

void Context::postInit()
{
    CodeLoader::postInit();
    DatabaseLoader::init();
}