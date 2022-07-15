#include "CodeLoader.h"

#include "Context.h"
#include "Utilities.h"

// Stores the current directory, and reverts it back to the original when it exits the current scope.
struct CurrentDirectoryGuard
{
    WCHAR currentDirectory[0x400];
    WCHAR dllDirectory[0x400];

    CurrentDirectoryGuard()
    {
        GetCurrentDirectoryW(_countof(currentDirectory), currentDirectory);
        GetDllDirectoryW(_countof(dllDirectory), dllDirectory);
    }

    ~CurrentDirectoryGuard()
    {
        SetCurrentDirectoryW(currentDirectory);
        SetDllDirectoryW(dllDirectory);
    }
};

constexpr const char* PRE_INIT_FUNC_NAMES[] = { "PreInit", "preInit", "pre_init" }; // Called in _scrt_common_main_seh
constexpr const char* INIT_FUNC_NAMES[] = { "Init", "init" }; // Called in WinMain
constexpr const char* POST_INIT_FUNC_NAMES[] = { "PostInit", "postInit", "post_init" }; // Called in WinMain after every Init
constexpr const char* D3D_INIT_FUNC_NAMES[] = { "D3DInit", "d3dInit", "d3d_init" }; // Called in D3D11CreateDeviceAndSwapChain
constexpr const char* ON_FRAME_FUNC_NAMES[] = { "OnFrame", "onFrame", "on_frame" }; // Called every frame before present

std::vector<std::wstring> CodeLoader::dllFilePaths;

std::vector<EventPair<InitEvent>> CodeLoader::initEvents;
std::vector<EventPair<InitEvent>> CodeLoader::postInitEvents;
std::vector<EventPair<D3DInitEvent>> CodeLoader::d3dInitEvents;
std::vector<OnFrameEvent*> CodeLoader::onFrameEvents;

VTABLE_HOOK(HRESULT, WINAPI, IDXGISwapChain, Present, UINT SyncInterval, UINT Flags)
{
    for (auto& onFrameEvent : CodeLoader::onFrameEvents)
        onFrameEvent(This);

    return originalIDXGISwapChainPresent(This, SyncInterval, Flags);
}

HOOK(HRESULT, WINAPI, D3D11CreateDeviceAndSwapChain, PROC_ADDRESS("d3d11.dll", "D3D11CreateDeviceAndSwapChain"),
    IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext)
{
    const HRESULT result = originalD3D11CreateDeviceAndSwapChain(
        pAdapter,
        DriverType,
        Software,
        Flags,
        pFeatureLevels,
        FeatureLevels,
        SDKVersion,
        pSwapChainDesc,
        ppSwapChain,
        ppDevice,
        pFeatureLevel,
        ppImmediateContext);

    if (FAILED(result))
        return result;

    if (!CodeLoader::d3dInitEvents.empty() && ppSwapChain && *ppSwapChain && 
        ppDevice && *ppDevice && *ppImmediateContext && ppImmediateContext)
    {
        CurrentDirectoryGuard guard;

        for (auto& event : CodeLoader::d3dInitEvents)
            event.run(*ppSwapChain, *ppDevice, *ppImmediateContext);
    }

    if (!CodeLoader::onFrameEvents.empty() && ppSwapChain && *ppSwapChain)
        INSTALL_VTABLE_HOOK(IDXGISwapChain, *ppSwapChain, Present, 8);

    return result;
}

// Gets called during _scrt_common_main_seh.
// Loads DLL mods and calls their "PreInit" functions.
void CodeLoader::init()
{
    processFilePaths(dllFilePaths, true);
    
    if (dllFilePaths.empty())
        return;

    LOG("DLL:")

    CurrentDirectoryGuard guard;

    for (auto& dllFilePath : dllFilePaths)
    {
        const std::wstring directoryPath = std::filesystem::path(dllFilePath).parent_path().wstring();

        SetCurrentDirectoryW(directoryPath.c_str());
        SetDllDirectoryW(directoryPath.c_str());

        const HMODULE module = LoadLibraryW(dllFilePath.c_str());

        if (!module)
        {
            LPWSTR buffer = nullptr;

            const DWORD msgSize = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buffer, 0, NULL);

            const std::wstring msg = L"Failed to load \"" + dllFilePath + L"\"\n" + std::wstring(buffer, msgSize);
            MessageBoxW(nullptr, msg.c_str(), L"DIVA Mod Loader", MB_OK);

            LocalFree(buffer);

            continue;
        }

        LOG(" - %ls", getRelativePath(dllFilePath).c_str())

        for (auto& preInitFuncName : PRE_INIT_FUNC_NAMES)
        {
            const FARPROC preInitEvent = GetProcAddress(module, preInitFuncName);

            if (preInitEvent)
                ((InitEvent*)preInitEvent)();
        }

        for (auto& initFuncName : INIT_FUNC_NAMES)
        {
            const FARPROC initEvent = GetProcAddress(module, initFuncName);

            if (initEvent)
                initEvents.push_back({ directoryPath, (InitEvent*)initEvent });
        }

        for (auto& postInitFuncName : POST_INIT_FUNC_NAMES)
        {
            const FARPROC postInitEvent = GetProcAddress(module, postInitFuncName);

            if (postInitEvent)
                postInitEvents.push_back({ directoryPath, (InitEvent*)postInitEvent });
        }

        for (auto& d3dInitFuncName : D3D_INIT_FUNC_NAMES)
        {
            const FARPROC d3dInitEvent = GetProcAddress(module, d3dInitFuncName);

            if (d3dInitEvent)
                d3dInitEvents.push_back({ directoryPath, (D3DInitEvent*)d3dInitEvent });
        }

        for (auto& onFrameFuncName : ON_FRAME_FUNC_NAMES)
        {
            const FARPROC onFrameEvent = GetProcAddress(module, onFrameFuncName);

            if (onFrameEvent)
                onFrameEvents.push_back((OnFrameEvent*)onFrameEvent);
        }
    }

    if (!d3dInitEvents.empty() || !onFrameEvents.empty())
        INSTALL_HOOK(D3D11CreateDeviceAndSwapChain);
}

// Gets called during WinMain.
// Calls "Init" and "PostInit" functions of DLL mods.
void CodeLoader::postInit()
{
    CurrentDirectoryGuard guard;

    for (auto& initEvent : initEvents)
        initEvent.run();

    for (auto& postInitEvent : postInitEvents)
        postInitEvent.run();
}