#include "Context.h"

#include "CodeLoader.h"
#include "DatabaseLoader.h"
#include "FileLoader.h"
#include "ModLoader.h"
#include "Patches.h"
#include "SaveData.h"
#include "SigScan.h"
#include "SpriteLoader.h"
#include "StrArray.h"
#include "Utilities.h"

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

SIG_SCAN
(
    sigCrtMain,
    0x140978288,
    "\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x30\xB9", 
    "xxxxxxxxxxx"
);

HOOK(int, WINAPI, CrtMain, sigCrtMain(), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Context::init();

    return originalCrtMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

SIG_SCAN
(
    sigWinMain,
    0x140978389,
    "\xE8\xCC\xCC\xCC\xCC\x8B\xD8\xE8\xCC\xCC\xCC\xCC\x84\xC0\x74\x50", 
    "x????xxx????xxxx"
); // call to function, E8 ?? ?? ?? ??

HOOK(int, WINAPI, WinMain, readInstrPtr(sigWinMain(), 0, 0x5), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
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
    SaveData::init();
    StrArray::init();
    SpriteLoader::init();

    INSTALL_HOOK(WinMain);
}

void Context::postInit()
{
    CodeLoader::postInit();
    DatabaseLoader::init();
}