#pragma once

// Signature scan in specified memory region
extern void* sigScan(const char* signature, const char* mask, size_t sigSize, void* memory, size_t memorySize);

// Signature scan in current process
extern void* sigScan(const char* signature, const char* mask, void* hint = nullptr);

// Automatically scanned signatures, these are expected to exist in all game versions
// sigValid is going to be false if any automatic signature scan fails
extern bool sigValid;

extern void* sigCrtMain();
extern void* sigWinMain();

extern void* sigOperatorNew();
extern void* sigOperatorDelete();
extern void* sigHeapCMallocAllocate();

extern void* sigInitRomDirectoryPaths();

extern void* sigInitSteamAPIManager();

extern void* sigResolveFilePath();
extern void* sigInitMdataMgr();

extern void* sigLoadFileFromCpk();

extern void* sigLoadStrArray();
extern void* sigGetStr();