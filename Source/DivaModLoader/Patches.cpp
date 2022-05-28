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
}
