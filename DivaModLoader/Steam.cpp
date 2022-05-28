#include "Context.h"

void initSteam()
{
    // Prevent SteamAPI_RestartAppIfNecessary.
    WRITE_MEMORY(0x1406051F4, uint8_t, 0xEB);

    // Create steam_appid.txt so the patch can actually function properly.
    {
        FILE* file = fopen("steam_appid.txt", "w");
        fprintf(file, "%d", 1761390);
        fclose(file);
    }
}