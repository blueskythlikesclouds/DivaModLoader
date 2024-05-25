#include "PvLoader.h"

static constexpr size_t DIFFICULTY_COUNT = 5;
static constexpr size_t EXTRA_COUNT = 2;

struct PvState
{
    bool difficultyStates[DIFFICULTY_COUNT][EXTRA_COUNT]{};
};

static std::unordered_map<uint32_t, PvState> pvStateMap;

static FUNCTION_PTR(void*, __fastcall, getPvData, 0x1404BC520, uint32_t id, uint32_t difficulty, uint32_t isExtra);

HOOK(void, __fastcall, SetPvStates, 0x1590DEBF0, void* a1)
{
    for (auto it = pvStateMap.begin(); it != pvStateMap.end();)
    {
        bool hasAnyDifficulty = false;

        for (uint32_t i = 0; i < DIFFICULTY_COUNT; i++)
        {
            for (uint32_t j = 0; j < EXTRA_COUNT; j++)
            {
                if (getPvData(it->first, i, j) != nullptr)
                {
                    it->second.difficultyStates[i][j] = true;
                    hasAnyDifficulty = true;
                }
            }
        }

        if (!hasAnyDifficulty)
            it = pvStateMap.erase(it);
        else
            ++it;
    }
}

HOOK(void, __fastcall, SpriteLoaderGetPvDifficultyStatesPtr, 0x1405811D0);

static bool invalidState = false;

bool* spriteLoaderGetPvDifficultyStatesPtrImp(uint32_t pvId, uint32_t difficultyMulBy1000)
{
    auto findResult = pvStateMap.find(pvId);
    return findResult != pvStateMap.end() ? findResult->second.difficultyStates[difficultyMulBy1000 / 1000] : &invalidState;
}

HOOK(void, __fastcall, PvLoaderGetPvDifficultyState, 0x1405807B4);

bool pvLoaderGetPvDifficultyStateImp(uint32_t pvId, uint32_t difficulty, uint32_t isExtra)
{
    auto findResult = pvStateMap.find(pvId);
    return findResult != pvStateMap.end() && findResult->second.difficultyStates[difficulty][isExtra];
}

HOOK(void, __fastcall, PvLoaderGetPvExists, 0x1405807E0);

bool pvLoaderGetPvExistsImp(uint32_t pvId)
{
    return pvStateMap.find(pvId) != pvStateMap.end();
}

HOOK(void, __fastcall, PvLoaderParseStart, 0x1404BB3C1);

static std::vector<uint32_t> pvIdStack;

uint32_t pvLoaderParseStartImp(const char* data, size_t length)
{
    size_t i = 0;
    uint32_t lastPvId = 0;
    while (i < length) 
    {
        if (data[i] == '#') 
        {
            while (i < length && data[i] != '\n')
                i++;

            i++;
        }
        else if (length - i > 3 && data[i] == 'p' && data[i + 1] == 'v' && data[i + 2] == '_') 
        {
            i += 3;
            size_t j = i;
            while (j < length && data[j] != '.')
                j++;

            uint32_t pvId = 0;
            for (; i < j; i++) 
            {
                pvId *= 10;
                pvId += std::clamp(data[i] - '0', 0, 9);
            }

            if (pvId != lastPvId && pvId != 0)
            {
                lastPvId = pvId;
                pvIdStack.push_back(pvId);
                pvStateMap.emplace(pvId, PvState{});
            }

            while (i < length && data[i] != '\n')
                i++;

            i++;
        }
        else
        {
            i++;
        }
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

uint32_t pvLoaderLoopImp()
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
    INSTALL_HOOK(SetPvStates);

    WRITE_CALL(originalSpriteLoaderGetPvDifficultyStatesPtr, implOfSpriteLoaderGetPvDifficultyStatesPtr);
    WRITE_NOP(reinterpret_cast<uint8_t*>(originalSpriteLoaderGetPvDifficultyStatesPtr) + 0xC, 0x3);
    WRITE_MEMORY(0x1405811EA, uint32_t, sizeof(bool));

    WRITE_CALL(originalPvLoaderGetPvDifficultyState, implOfPvLoaderGetPvDifficultyState);
    WRITE_NOP(reinterpret_cast<uint8_t*>(originalPvLoaderGetPvDifficultyState) + 0xC, 0xA);

    WRITE_JUMP(originalPvLoaderGetPvExists, implOfPvLoaderGetPvExists);

    WRITE_CALL(originalPvLoaderParseStart, implOfPvLoaderParseStart);
    WRITE_NOP(reinterpret_cast<uint8_t*>(originalPvLoaderParseStart) + 0xC, 0x3);
    
    WRITE_JUMP(originalPvLoaderParseLoop, implOfPvLoaderParseLoop);
}
