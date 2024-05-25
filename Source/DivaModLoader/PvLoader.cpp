#include "PvLoader.h"

HOOK(void, __fastcall, PvLoaderParseStart, 0x1404BB3C1);

static std::vector<uint32_t> pvIdStack;

uint32_t pvLoaderParseStartImp(const char* data, size_t length)
{
    size_t i = 0;
    uint32_t lastPvId = 0;
    while (i < length) 
    {
        // Skip whitespace at the start of line.
        while (i < length && (data[i] == '\t' || data[i] == '\n' || data[i] == '\r' || data[i] == ' '))
            i++;

        if (length - i > 3 && data[i] == 'p' && data[i + 1] == 'v' && data[i + 2] == '_') 
        {
            i += 3;

            uint32_t pvId = 0;
            while(i < length && std::isdigit(data[i]))
            {
                pvId *= 10;
                pvId += data[i] - '0';
                i++;
            }

            if (pvId != lastPvId && pvId != 0)
            {
                lastPvId = pvId;
                pvIdStack.push_back(pvId);
            }
        }

        // Move onto the next line.
        while (i < length && data[i] != '\n' && data[i] != '\r')
            i++;
    }

    if (!pvIdStack.empty())
    {
        uint32_t pvId = pvIdStack.back();
        pvIdStack.pop_back();
        return pvId;
    }

    return 0xFFFFFFFF;
}

HOOK(void, __fastcall, PvLoaderParseLoop, 0x1404BB5D6);

uint32_t pvLoaderParseLoopImp()
{
    if (!pvIdStack.empty()) 
    {
        uint32_t pvId = pvIdStack.back();
        pvIdStack.pop_back();
        return pvId;
    }
    
    return 0xFFFFFFFF;
}

void PvLoader::init()
{
    // Skip if checks that always return true but would access out of bounds data due to large IDs regardless
    WRITE_NOP(0x1405807C2, 0xE);
    WRITE_MEMORY(0x1405811E0, uint8_t, 0x90, 0x90, 0x90, 0xEB);
    WRITE_MEMORY(0x1405807F0, uint8_t, 0x90, 0x90, 0x90, 0xEB);
    WRITE_MEMORY(0x140580813, uint8_t, 0x90, 0x90, 0x90, 0xEB);
    WRITE_MEMORY(0x140580836, uint8_t, 0x90, 0x90, 0x90, 0xEB);
    WRITE_MEMORY(0x140580860, uint8_t, 0x90, 0x90, 0x90, 0xEB);
    WRITE_MEMORY(0x140580883, uint8_t, 0x90, 0x90, 0x90, 0xEB);

    // Prevent truncation to u16 when using MM+ UI, there's enough space for an u32 since the next element is 4 byte aligned
    WRITE_NOP(0x1406DE563, 1);
    WRITE_NOP(0x1406DEA21, 1);
    WRITE_NOP(0x1406DEA28, 3);
    WRITE_MEMORY(0x1406DF6F4, uint8_t, 0x90, 0x8B);
    WRITE_NOP(0x1406DF748, 1);

    // Scan the pv_db file before reading it to not waste time looking for entries that don't exist in the file
    WRITE_CALL(originalPvLoaderParseStart, implOfPvLoaderParseStart);
    WRITE_NOP(reinterpret_cast<uint8_t*>(originalPvLoaderParseStart) + 0xC, 0x3);
    
    WRITE_JUMP(originalPvLoaderParseLoop, implOfPvLoaderParseLoop);
}
