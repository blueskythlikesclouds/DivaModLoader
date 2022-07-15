#include "Patches.h"
#include "SigScan.h"

void Patches::init()
{
    // Prevent SteamAPI_RestartAppIfNecessary.
    WRITE_MEMORY((uint8_t*)sigInitSteamAPIManager() + 0x24, uint8_t, 0xEB);

    // Create steam_appid.txt so the patch can actually function properly.
    {
        FILE* file = fopen("steam_appid.txt", "w");
        fprintf(file, "%d", 1761390);
        fclose(file);
    }

    // Enable loose folder support for 1.01.
    void* romCheckAddr1 = sigScan("\x48\x8B\x43\x10\x48\x89\xB4\x24\x00\x00\x00\x00\x48\x83\xF8\x04\x72\x41\x48\x8D\x34\x38\xBA\x00\x00\x00\x00\x4C\x8D\x46\xFD\x48\x8B\xCF\x4C\x2B\xC7\xE8\x00\x00\x00\x00\x48\x85\xC0\x74\x24", "xxxxxxxx????xxxxxxxxxxx????xxxxxxxxxxx????xxxxx", (void*)0x14016BFB6);
    void* romCheckAddr2 = sigScan("\x48\x8D\x15\x00\x00\x00\x00\x48\x89\xF1\xE8\x00\x00\x00\x00\x48\x85\xC0\x74\x12\x31\xC0\x48\x8B\x5C\x24\x00\x48\x8B\x74\x24\x00\x48\x83\xC4\x20\x5F\xC3", "xxx????xxxx????xxxxxxxxxxx?xxxx?xxxxxx", (void*)0x152243F0A);

    if (romCheckAddr1) WRITE_MEMORY((char*)romCheckAddr1 + 0x10, uint8_t, 0xEB);
    if (romCheckAddr2) WRITE_MEMORY((char*)romCheckAddr2 + 0x12, uint8_t, 0xEB);
}
