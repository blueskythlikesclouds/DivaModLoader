#include "SigScan.h"
#include <Psapi.h>

FORCEINLINE void* sigScan(const char* signature, const char* mask, size_t sigSize, void* memory, const size_t memorySize)
{
    if (sigSize == 0)
        sigSize = strlen(mask);

    for (size_t i = 0; i < memorySize; i++)
    {
        char* currMemory = (char*)memory + i;

        size_t j;
        for (j = 0; j < sigSize; j++)
        {
            if (mask[j] != '?' && signature[j] != currMemory[j])
                break;
        }

        if (j == sigSize)
            return currMemory;
    }

    return nullptr;
}

MODULEINFO moduleInfo;

const MODULEINFO& getModuleInfo()
{
    if (moduleInfo.SizeOfImage)
        return moduleInfo;

    ZeroMemory(&moduleInfo, sizeof(MODULEINFO));
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(MODULEINFO));

    return moduleInfo;
}

FORCEINLINE void* sigScan(const char* signature, const char* mask, void* hint)
{
    const MODULEINFO& info = getModuleInfo();
    const size_t sigSize = strlen(mask);

    // Ensure hint address is within the process memory region so there are no crashes.
    if ((hint >= info.lpBaseOfDll) && ((char*)hint + sigSize <= (char*)info.lpBaseOfDll + info.SizeOfImage))
    {
        void* result = sigScan(signature, mask, sigSize, hint, sigSize);

        if (result)
            return result;
    }

    return sigScan(signature, mask, sigSize, info.lpBaseOfDll, info.SizeOfImage);
}

#define SIG_SCAN(x, y, ...) \
    void* x(); \
    void* x##Addr = x(); \
    void* x() \
    { \
        constexpr const char* x##Data[] = { __VA_ARGS__ }; \
        constexpr size_t x##Size = _countof(x##Data); \
        if (!x##Addr) \
        { \
            if constexpr (x##Size == 2) \
            { \
                x##Addr = sigScan(x##Data[0], x##Data[1], (void*)(y)); \
                if (x##Addr) \
                    return x##Addr; \
            } \
            else \
            { \
                for (int i = 0; i < x##Size; i += 2) \
                { \
                    x##Addr = sigScan(x##Data[i], x##Data[i + 1], (void*)(y)); \
                    if (x##Addr) \
                        return x##Addr; \
                } \
            } \
            sigValid = false; \
        } \
        return x##Addr; \
    }

bool sigValid = true;

SIG_SCAN(sigCrtMain, 0x140978288, "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x30\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\x00\x00\x00\x00\x40\x32\xFF\x40\x88\x7C\x24\x00\xE8\x00\x00\x00\x00\x8A\xD8\x8B\x0D\x00\x00\x00\x00\x83\xF9\x01\x0F\x84\x00\x00\x00\x00\x85\xC9\x75\x4A\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x48\x8D\x15\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x85\xC0\x74\x0A\xB8\x00\x00\x00\x00\xE9\x00\x00\x00\x00", "xxxx?xxxxxx????x????xxxx????xxxxxxx?x????xxxx????xxxxx????xxxxxx????????xxx????xxx????x????xxxxx????x????")
SIG_SCAN(sigWinMain, 0x1402C33F0, "\x40\x55\x53\x57\x41\x54\x41\x55\x48\x8D\xAC\x24\x00\x00\x00\x00\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x2B\xE0\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\x48\x8B\xD9\xBA\x00\x00\x00\x00\x33\xC9\x45\x8B\xE1\xFF\x15\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x4C\x8B\xC0\x48\x8D\x95\x00\x00\x00\x00\x48\x8D\x8D\x00\x00\x00\x00", "xxxxxxxxxxxx????x????x????xxxxxx????xxxxxx????xxxx????xxxxxxx????xx????xxxxxx????xxx????")
SIG_SCAN(sigOperatorNew, 0x1409777D0, "\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\xEB\x0F\x48\x8B\xCB\xE8\x00\x00\x00\x00\x85\xC0\x74\x13\x48\x8B\xCB\xE8\x00\x00\x00\x00\x48\x85\xC0\x74\xE7\x48\x83\xC4\x20\x5B\xC3\x48\x83\xFB\xFF\x74\x06\xE8\x00\x00\x00\x00\xCC\xE8\x00\x00\x00\x00\xCC\x40\x53", "xxxxxxxxxxxxxxx????xxxxxxxx????xxxxxxxxxxxxxxxxxx????xx????xxx")
SIG_SCAN(sigOperatorDelete, 0x1409B1E90, "\x48\x85\xC9\x74\x37\x53\x48\x83\xEC\x20\x4C\x8B\xC1\x33\xD2\x48\x8B\x0D\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x85\xC0\x75\x17\xE8\x00\x00\x00\x00\x48\x8B\xD8\xFF\x15\x00\x00\x00\x00\x8B\xC8\xE8\x00\x00\x00\x00\x89\x03\x48\x83\xC4\x20\x5B\xC3", "xxxxxxxxxxxxxxxxxx????xx????xxxxx????xxxxx????xxx????xxxxxxxx")
SIG_SCAN(sigHeapCMallocAllocate, 0x1404402B0, "\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x20\x0F\xB6\x05\x00\x00\x00\x00\x49\x8B\xE8\x48\x63\xF9\x48\x8B\xF2\x84\xC0\x75\x0C\xE8\x00\x00\x00\x00\x0F\xB6\x05\x00\x00\x00\x00", "xxxx?xxxx?xxxxxxxx????xxxxxxxxxxxxxx????xxx????")
SIG_SCAN(sigInitRomDirectoryPaths, 0x1402A23E0, "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8B\xEC\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\xF0\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x33\xF6\x48\x89\x75\xD0\x48\x89\x75\xE0\xBF\x00\x00\x00\x00\x48\x89\x7D\xE8\x40\x88\x75\xD0\x45\x33\xC0\x48\x8D\x15\x00\x00\x00\x00\x48\x8D\x4D\xD0\xE8\x00\x00\x00\x00\x90\x44\x8D\x46\x02\x48\x8D\x15\x00\x00\x00\x00\x48\x8D\x4D\xD0\xE8\x00\x00\x00\x00\x89\x75\xC0\x4C\x8D\x25\x00\x00\x00\x00\x48\x8D\x5E\xFF\x0F\x1F\x80\x00\x00\x00\x00", "xxxx?xxxx?xxxx?xxxxxxxxxxxxxxx????xxx????xxxxxxxxxx????x????xxxxxxxxxxx????xxxxxxxxxxxxxx????xxxxx????xxxxxxxx????xxxxx????xxxxxx????xxxxxxx????")
SIG_SCAN(sigInitSteamAPIManager, 0x1405FE5E0, "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x20\x48\x8B\xF9\x80\x79\x08\x00\x0F\x85\x00\x00\x00\x00\xB9\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x84\xC0\x74\x0F\xC6\x47\x08\x00\x48\x8B\x5C\x24\x00\x48\x83\xC4\x20\x5F\xC3", "xxxx?xxxxxxxxxxxxxx????x????xx????xxxxxxxxxxxx?xxxxxx")
SIG_SCAN(sigResolveFilePath, 0x1402A5330, "\x48\x89\x5C\x24\x00\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xEC\x40\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x44\x24\x00\x4C\x8B\xE2\x4C\x8B\xF9\x33\xC0\x0F\x57\xC9\xF3\x0F\x7F\x4C\x24\x00\x48\x89\x44\x24\x00\x48\x8D\x54\x24\x00\xE8\x00\x00\x00\x00\x83\xF8\x01\x75\x46\x49\x8B\xCF\xE8\x00\x00\x00\x00\x84\xC0\x74\x28\x4D\x85\xE4\x0F\x84\x00\x00\x00\x00\x4D\x3B\xE7\x0F\x84\x00\x00\x00\x00\x49\x8B\xD7\x49\x83\x7F\x00\x00\x72\x03\x49\x8B\x17", "xxxx?xxxxxxxxxxxxxxxxxx????xxxxxxx?xxxxxxxxxxxxxxxx?xxxx?xxxx?x????xxxxxxxxx????xxxxxxxxx????xxxxx????xxxxxx??xxxxx")
SIG_SCAN(sigInitMdataMgr, 0x14043E050, "\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x41\x54\x41\x56\x41\x57\x48\x83\xEC\x60\x48\x8B\x44\x24\x00\x48\x89\x44\x24\x00\x4C\x8D\x25\x00\x00\x00\x00\x4C\x89\x64\x24\x00\x49\x8B\xCC", "xxxx?xxxx?xxxx?xxxx?xxxxxxxxxxxxxx?xxxx?xxx????xxxx?xxx")
SIG_SCAN(sigLoadFileFromCpk, 0x1401717C0, "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\x27\x45\x0F\xB6\xE8\x44\x0F\xB6\xE2\x48\x8B\xF1\x48\x83\x3D\x00\x00\x00\x00\x00\x75\x07\x33\xC0\xE9\x00\x00\x00\x00", "xxxx?xxxx?xxxx?xxxxxxxxxxxxx?xxx????xxx????xxxxxxxxxxxxxxxxxxxxx?????xxxxx????")
SIG_SCAN(sigLoadStrArray, 0x1402397E0, "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x60\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x44\x24\x00\x48\x83\x3D\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\x33\xC0\x48\x89\x44\x24\x00\x48\xC7\x44\x24\x00\x00\x00\x00\x00\x48\x89\x44\x24\x00\x88\x44\x24\x30\x44\x8D\x40\x04\x48\x8D\x15\x00\x00\x00\x00\x48\x8D\x4C\x24\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x4C\x8B\xC8\x48\xC7\xC7\x00\x00\x00\x00", "xxxx?xxxxxxxx????xxxxxxx?xxx?????xx????xxxxxx?xxxx?????xxxx?xxxxxxxxxxx????xxxx?x????x????xxxxxx????")
SIG_SCAN(sigGetStr, 0x14F8C43B0, "\x48\x8B\x15\x00\x00\x00\x00\x48\x85\xD2\x74\x10\x81\xF9\x00\x00\x00\x00\x7D\x08\x48\x63\xC1\x48\x8B\x04\xC2\xC3\x31\xC0\xC3", "xxx????xxxxxxx????xxxxxxxxxxxxx")
SIG_SCAN(sigGetModuleName, 0x155E2FDEA, "\xE8\x00\x00\x00\x00\x48\x8B\x4C\x24\x00\x48\x31\xE1\xE8\x00\x00\x00\x00\x4C\x8D\x5C\x24\x00\x49\x8B\x5B\x20\x49\x8B\x73\x28\x4C\x89\xDC\x5F\xC3\xE8\x00\x00\x00\x00\xCC\xCC\x0F\x1F\x00", "x????xxxx?xxxx????xxxx?xxxxxxxxxxxxxx????xxxxx")