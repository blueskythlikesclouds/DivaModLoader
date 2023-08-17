#include "Patches.h"
#include "SigScan.h"

HOOK(bool, __fastcall, SteamAPI_RestartAppIfNecessary, PROC_ADDRESS("steam_api64.dll", "SteamAPI_RestartAppIfNecessary"), uint32_t appid)
{
    originalSteamAPI_RestartAppIfNecessary(appid);
    FILE* file = fopen("steam_appid.txt", "w");
    fprintf(file, "%d", appid);
    fclose(file);
    return false;
}

SIG_SCAN
(
    sigRomCheck1,
    0x14016BFB6,
    "\x48\x8B\x43\x10\x48\x89\xB4\x24\xA8\x00\x00\x00\x48\x83\xF8\x04\x72\x41", 
    "xxxxxxxxxxxxxxxxxx"
);

SIG_SCAN
(
    sigRomCheck2,
    0x151C9BACC,
    "\x48\x8D\x15\xCC\xCC\xCC\xCC\x48\x89\xF1\xE8", 
    "xxx????xxxx"
);

SIG_SCAN
(
    sigModuleIdLimit1,
    0x14E7DCDB0,
    "\x8B\x11\x31\xC0\x8D", 
    "xxxxx"
);

SIG_SCAN
(
    sigModuleIdLimit2,
    0x14E810860,
    "\x8D\x42\x0C\x45\x31", 
    "xxxxx"
);

SIG_SCAN
(
    sigCosLimit1,
    0x14067F443,
    "\x76\x03\x41\x8B\xDE", 
    "xxxxx"
);

SIG_SCAN
(
    sigCosLimit2,
    0x1587FCBA4,
    "\x44\x0F\x47\xC9\x41\x0F\x10\x02", 
    "xxxxxxxx"
);

void Patches::init()
{
    // Prevent SteamAPI_RestartAppIfNecessary.
    INSTALL_HOOK(SteamAPI_RestartAppIfNecessary);

    // Enable loose folder support.
    WRITE_MEMORY((uint8_t*)sigRomCheck1() + 0x10, uint8_t, 0xEB);
    WRITE_MEMORY((uint8_t*)sigRomCheck2() + 0x12, uint8_t, 0xEB);

    // Remove module ID limit of 1035.
    WRITE_MEMORY((uint8_t*)sigModuleIdLimit1() + 0xD, uint8_t, 0x89, 0xD0, 0xC3);
    WRITE_NOP((uint8_t*)sigModuleIdLimit2() + 0xB, 4);

    // Remove COS limit of 498.
    WRITE_MEMORY(sigCosLimit1(), uint8_t, 0xEB);
    WRITE_NOP(sigCosLimit2(), 4);

    // Remove texture ID limit of 1048575.
    // FIXME: This breaks module texture replacements!
    //WRITE_NOP(0x140461021, 6);
    //WRITE_NOP(0x140461435, 6);
    //WRITE_NOP(0x140462608, 6);
    //WRITE_NOP(0x140462C66, 6);
}
