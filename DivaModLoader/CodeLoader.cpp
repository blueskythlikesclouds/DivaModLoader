#include "Context.h"
#include "Utilities.h"

typedef void DllEvent();

std::vector<std::wstring> dllFilePaths;

void initCodeLoader()
{
    processFilePaths(dllFilePaths, true);

    if (dllFilePaths.empty())
        return;

    std::vector<DllEvent*> postInitEvents;

    WCHAR currentDirectory[0x400];
    WCHAR dllDirectory[0x400];

    GetCurrentDirectoryW(_countof(currentDirectory), currentDirectory);
    GetDllDirectoryW(_countof(dllDirectory), dllDirectory);

    LOG("DLL:")

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

        const FARPROC initEvent = GetProcAddress(module, "Init");

        if (initEvent)
            ((DllEvent*)initEvent)();

        const FARPROC postInitEvent = GetProcAddress(module, "PostInit");

        if (postInitEvent)
            postInitEvents.push_back((DllEvent*)postInitEvent);
    }

    for (auto& dllEvent : postInitEvents)
        dllEvent();

    SetCurrentDirectoryW(currentDirectory);
    SetDllDirectoryW(dllDirectory);
}
