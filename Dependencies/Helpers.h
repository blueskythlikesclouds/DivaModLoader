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
        void* writeMemLoc = (void*)(location); \
        const type writeMemData[] = { __VA_ARGS__ }; \
        DWORD writeMemOldProtect; \
        VirtualProtect(writeMemLoc, sizeof(writeMemData), PAGE_EXECUTE_READWRITE, &writeMemOldProtect); \
        memcpy(writeMemLoc, writeMemData, sizeof(writeMemData)); \
        VirtualProtect(writeMemLoc, sizeof(writeMemData), writeMemOldProtect, &writeMemOldProtect); \
    } while(0)

#define WRITE_JUMP(location, function) \
    do { \
        size_t writeJmpLoc = (size_t)(location); \
        WRITE_MEMORY(writeJmpLoc, uint8_t, 0x48, 0xB8); \
        WRITE_MEMORY(writeJmpLoc + 2, uint64_t, (uint64_t)(function)); \
        WRITE_MEMORY(writeJmpLoc + 10, uint8_t, 0xFF, 0xE0); \
    } while(0)
	
#define WRITE_CALL(location, function) \
    do { \
        size_t writeCallLoc = (size_t)(location); \
        WRITE_MEMORY(writeCallLoc, uint8_t, 0x48, 0xB8); \
        WRITE_MEMORY(writeCallLoc + 2, uint64_t, (uint64_t)(function)); \
        WRITE_MEMORY(writeCallLoc + 10, uint8_t, 0xFF, 0xD0); \
    } while(0)

#define WRITE_NOP(location, count) \
    do { \
        void* writeNopLoc = (void*)(location); \
        size_t writeNopCount = (size_t)(count); \
        DWORD writeNopOldProtect; \
        VirtualProtect(writeNopLoc, writeNopCount, PAGE_EXECUTE_READWRITE, &writeNopOldProtect); \
        for (size_t i = 0; i < writeNopCount; i++) \
            *((uint8_t*)writeNopLoc + i) = 0x90; \
        VirtualProtect(writeNopLoc, writeNopCount, writeNopOldProtect, &writeNopOldProtect); \
    } while(0)
