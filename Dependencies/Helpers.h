#pragma once

#define _CONCAT2(x, y) x##y
#define CONCAT2(x, y) _CONCAT(x, y)
#define INSERT_PADDING(length) \
    uint8_t CONCAT2(pad, __LINE__)[length]

#define ASSERT_OFFSETOF(type, field, offset) \
    static_assert(offsetof(type, field) == offset, "offsetof assertion failed")

#define ASSERT_SIZEOF(type, size) \
    static_assert(sizeof(type) == size, "sizeof assertion failed")

#ifdef BASE_ADDRESS
const HMODULE MODULE_HANDLE = GetModuleHandle(nullptr);

#define ASLR(address) \
    ((size_t)MODULE_HANDLE + (size_t)address - (size_t)BASE_ADDRESS)
#endif

#define FUNCTION_PTR(returnType, callingConvention, function, location, ...) \
    returnType (callingConvention *function)(__VA_ARGS__) = (returnType(callingConvention*)(__VA_ARGS__))(location)

#define PROC_ADDRESS(libraryName, procName) \
    GetProcAddress(LoadLibrary(TEXT(libraryName)), procName)

#define HOOK(returnType, callingConvention, functionName, location, ...) \
    typedef returnType callingConvention functionName##Delegate(__VA_ARGS__); \
    functionName##Delegate* original##functionName = (functionName##Delegate*)(location); \
    returnType callingConvention implOf##functionName(__VA_ARGS__)

#define INSTALL_HOOK(functionName) \
    do { \
        DetourTransactionBegin(); \
        DetourUpdateThread(GetCurrentThread()); \
        DetourAttach((void**)&original##functionName, implOf##functionName); \
        DetourTransactionCommit(); \
    } while(0)

#define VTABLE_HOOK(returnType, callingConvention, className, functionName, ...) \
    typedef returnType callingConvention className##functionName(className* This, __VA_ARGS__); \
    className##functionName* original##className##functionName; \
    returnType callingConvention implOf##className##functionName(className* This, __VA_ARGS__)

#define INSTALL_VTABLE_HOOK(className, object, functionName, functionIndex) \
    do { \
        if (original##className##functionName == nullptr) \
        { \
            original##className##functionName = (*(className##functionName***)object)[functionIndex]; \
            DetourTransactionBegin(); \
            DetourUpdateThread(GetCurrentThread()); \
            DetourAttach((void**)&original##className##functionName, implOf##className##functionName); \
            DetourTransactionCommit(); \
        } \
    } while(0)

#define WRITE_MEMORY(location, type, ...) \
    do { \
        const type data[] = { __VA_ARGS__ }; \
        DWORD oldProtect; \
        VirtualProtect((void*)(location), sizeof(data), PAGE_EXECUTE_READWRITE, &oldProtect); \
        memcpy((void*)(location), data, sizeof(data)); \
        VirtualProtect((void*)(location), sizeof(data), oldProtect, &oldProtect); \
    } while(0)

#define WRITE_JUMP(location, function) \
    do { \
        WRITE_MEMORY((size_t)(location), uint8_t, 0x48, 0xB8); \
        WRITE_MEMORY((size_t)(location) + 2, uint64_t, (uint64_t)(function)); \
        WRITE_MEMORY((size_t)(location) + 10, uint8_t, 0xFF, 0xE0); \
    } while(0)
	
#define WRITE_CALL(location, function) \
    do { \
        WRITE_MEMORY((size_t)(location), uint8_t, 0x48, 0xB8); \
        WRITE_MEMORY((size_t)(location) + 2, uint64_t, (uint64_t)(function)); \
        WRITE_MEMORY((size_t)(location) + 10, uint8_t, 0xFF, 0xD0); \
    } while(0)

#define WRITE_NOP(location, count) \
    do { \
        DWORD oldProtect; \
        VirtualProtect((void*)(location), (size_t)(count), PAGE_EXECUTE_READWRITE, &oldProtect); \
        for (size_t i = 0; i < (size_t)(count); i++) \
            *((uint8_t*)(location) + i) = 0x90; \
        VirtualProtect((void*)(location), (size_t)(count), oldProtect, &oldProtect); \
    } while(0)