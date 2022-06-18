#include "FileLoader.h"
#include "Allocator.h"

// This piece of code here only applies to 1.01. For some reason, not all TXT files can be loaded
// outside CPKs in this version, so we need to do it manually.

struct CpkFileHandle
{
    char type[0x100]; // "criloader"
    void* data; // 0x100
    size_t dataSize; // 0x108
    uint8_t gap110[0x18];
};

HOOK(CpkFileHandle*, __fastcall, OpenFileFromCpk, sigLoadFileFromCpk(), const char* fileName, bool a2, bool a3)
{
    // I don't know what these arguments mean, but they are utilized by files
    // that we are interested in manually loading.
    if (a2 && a3)
    {
        FILE* file = fopen(fileName, "rb");
        if (file)
        {
            // Replicate the exact same data structure as the original function.
            CpkFileHandle* handle = (CpkFileHandle*)operatorNew(sizeof(CpkFileHandle));
            ZeroMemory(handle, sizeof(*handle));

            strcpy(handle->type, "criloader");

            // Load entirety of the file at once.
            fseek(file, 0, SEEK_END);
            handle->dataSize = ftell(file);
            fseek(file, 0, SEEK_SET);
            handle->data = heapCMallocAllocate(5, handle->dataSize, "cri file extract buffer");
            fread(handle->data, handle->dataSize, 1, file);
            fclose(file);

            return handle;
        }
    }

    return originalOpenFileFromCpk(fileName, a2, a3);
}

void FileLoader::init()
{
    INSTALL_HOOK(OpenFileFromCpk);
}
