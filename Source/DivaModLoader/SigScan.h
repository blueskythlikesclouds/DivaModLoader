#pragma once

#include <Psapi.h>

FORCEINLINE const MODULEINFO& getModuleInfo()
{
    static MODULEINFO moduleInfo;

    if (moduleInfo.SizeOfImage)
        return moduleInfo;

    ZeroMemory(&moduleInfo, sizeof(MODULEINFO));
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(MODULEINFO));

    return moduleInfo;
}

// Signature scan in specified memory region
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

// Signature scan in current process
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

// Automatically scanned signatures, these are expected to exist in all game versions
// sigValid is going to be false if any automatic signature scan fails
inline bool sigValid = true;

// Automatically scanned signature
#define SIG_SCAN(x, y, ...) \
    FORCEINLINE void* x(); \
    inline void* x##Addr = x(); \
    FORCEINLINE void* x() \
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