#include "ThumbnailLoader.h"

#include "SigScan.h"
#include "Utilities.h"
#include "Types.h"

struct string_range
{
    char* start;
    char* end;

    string_range() 
    {
        start = nullptr;
        end = nullptr;
    }

    string_range(char* str)
    {
        start = str;
        end = str + strlen(str);
    }

    string_range(char* str, size_t length) 
    {
        start = str;
        end = str + length;
    }
};

struct PvSpriteId
{
    void* pv;
    uint32_t set;
    uint32_t bg[4];
    uint32_t jk[4];
    uint32_t logo[4];
    uint32_t tmb[4];
};

struct SpriteInfo {
    uint32_t id;
    string_range name;
    uint16_t index;
    uint16_t set_index;
};

SIG_SCAN
(
    sigLoadPvSpriteIds,
    0x140580DF0,
    "\x48\x89\x5C\x24\x10\x48\x89\x74\x24\x18\x48\x89\x7C\x24\x20\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\xF0\x48\x81\xEC\x10\x01\x00\x00\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x33\xC4\x48"
    "\x89\x45\x00\x4C\x8B\xF9\x48",
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxx"
);

SIG_SCAN
(
    sigLoadSprSet,
    0x14027F777,
    "\xE8\x00\x00\x00\x00\x8B\x4F\xA0",
    "x????xxx"
);

SIG_SCAN
(
    sigLoadSprSetFinish,
    0x14023C405,
    "\xE8\x00\x00\x00\x00\x84\xC0\x75\xDC",
    "x????xxxx"
);

SIG_SCAN
(
    sigGetSpriteInfo,
    0x1405BC8F0,
    "\x41\x56\x48\x83\xEC\x30\x48\x89\x5C\x24\x40\x48\x8D\x0D\xCC\xCC\xCC\xCC\x48\x89\x7C\x24\x28\x4C\x89\x7C\x24\x20\x4C",
    "xxxxxxxxxxxxxx????xxxxxxxxxxx"
);

static FUNCTION_PTR(void, __fastcall, loadSprSet, readInstrPtr(sigLoadSprSet(), 0, 5), uint32_t setId, string_range& a2);
static FUNCTION_PTR(bool, __fastcall, loadSprSetFinish, readInstrPtr(sigLoadSprSetFinish(), 0, 5), uint32_t setId);
static FUNCTION_PTR(SpriteInfo *, __fastcall, getSpriteInfo, sigGetSpriteInfo(), void* a1, string_range& name);
static FUNCTION_PTR(uint32_t *, __fastcall, getSpriteSetByIndex, 0x1405BC680, void* a1, uint32_t index);

static void loadSprSetWait(std::set<uint32_t> pendingSets) 
{
    while (pendingSets.size() > 0)
    {
        for (auto it = pendingSets.begin(); it != pendingSets.end();)
        {
            if (loadSprSetFinish(*it) == 0)
                it = pendingSets.erase(it);
            else 
                it++;
        }

        std::this_thread::yield();
    }
}

HOOK(void, __fastcall, LoadPvSpriteIds, sigLoadPvSpriteIds(), uint64_t a1)
{
    originalLoadPvSpriteIds(a1);

    std::set<uint32_t> pendingSets;
    auto sprites = (prj::map<int, PvSpriteId> *)(a1 + 0x330);
    for (auto it = sprites->begin(); it != sprites->end(); it++)
    {
        char buf[64];
        int length;
        uint32_t set;
        uint32_t set_ex;
        SpriteInfo *spr;
        string_range name;

        length = sprintf(buf, "SPR_SEL_PVTMB_%03d", it->first);
        name = string_range(buf, length);
        spr = getSpriteInfo(nullptr, name);
        if (spr->id == (uint32_t)-1)
            continue;

        set = *getSpriteSetByIndex(nullptr, spr->set_index);
        if (set != (uint32_t)-1 && pendingSets.find(set) == pendingSets.end()) {
            name = string_range();
            loadSprSet(set, name);
            pendingSets.insert(set);
        }

        length = sprintf(buf, "SPR_SEL_PVTMB_%03d_EX", it->first);
        name = string_range(buf, length);
        spr = getSpriteInfo(nullptr, name);
        if (spr->id != (uint32_t)-1)
        {
            set_ex = *getSpriteSetByIndex(nullptr, spr->set_index);
            if (set_ex != (uint32_t)-1 && set_ex != set && pendingSets.find(set_ex) == pendingSets.end()) {
                name = string_range();
                loadSprSet(set, name);
                pendingSets.insert(set);
            }
        }
    }

    if (!pendingSets.empty())
    {
        std::thread t(loadSprSetWait, pendingSets);
        t.detach();
    }
}

void ThumbnailLoader::init() 
{
    INSTALL_HOOK(LoadPvSpriteIds);
}