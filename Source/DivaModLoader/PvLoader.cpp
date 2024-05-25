#include "PvLoader.h"

static constexpr size_t DIFFICULTY_COUNT = 5;
static constexpr size_t ATTRIBUTE_COUNT = 2;

struct PvState
{
    bool difficultyStates[DIFFICULTY_COUNT][ATTRIBUTE_COUNT]{};
};

static std::unordered_map<uint32_t, PvState> pvStateMap;

static FUNCTION_PTR(void*, __fastcall, getPvData, 0x1404BC570, uint32_t id, uint32_t difficulty, uint32_t attribute);

HOOK(void, __fastcall, SetPvStates, 0x1590DEBF0, void* a1)
{
    for (auto it = pvStateMap.begin(); it != pvStateMap.end();)
    {
        bool hasAnyDifficulty = false;

        for (uint32_t i = 0; i < DIFFICULTY_COUNT; i++)
        {
            for (uint32_t j = 0; j < ATTRIBUTE_COUNT; j++)
            {
                auto& difficultyState = it->second.difficultyStates[i][j];
                difficultyState = getPvData(it->first, i, j) != nullptr;

                if (difficultyState)
                    hasAnyDifficulty = true;
            }

            // TODO: Find out why not doing this causes a crash with songs that only have Extra Extreme
            if (!it->second.difficultyStates[i][0] && it->second.difficultyStates[i][1])
                it->second.difficultyStates[i][0] = true;
        }

        if (!hasAnyDifficulty)
            it = pvStateMap.erase(it);
        else
            ++it;
    }
}

HOOK(void, __fastcall, SpriteLoaderGetPvDifficultyStatesPtr, 0x1405811D0);

static bool invalidAttributes[ATTRIBUTE_COUNT]{};

bool* spriteLoaderGetPvDifficultyStatesPtrImp(uint32_t pvId, uint32_t difficulty)
{
    auto findResult = pvStateMap.find(pvId);
    return findResult != pvStateMap.end() ? findResult->second.difficultyStates[difficulty / 2000] : invalidAttributes;
}

HOOK(void, __fastcall, PvLoaderGetPvDifficultyState, 0x1405807B4);

bool pvLoaderGetPvDifficultyStateImp(uint32_t pvId, uint32_t difficulty, uint32_t attribute)
{
    auto findResult = pvStateMap.find(pvId);
    return findResult != pvStateMap.end() && findResult->second.difficultyStates[difficulty][attribute];
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
                pvStateMap.emplace(pvId, PvState{});
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

    // Redirect sprite loader's PV existence checks to the DML map
    WRITE_CALL(originalSpriteLoaderGetPvDifficultyStatesPtr, implOfSpriteLoaderGetPvDifficultyStatesPtr);
    WRITE_NOP(reinterpret_cast<uint8_t*>(originalSpriteLoaderGetPvDifficultyStatesPtr) + 0xC, 0x3);

    // Shift 1 instead of 1000 because the extra difficulty bools are right after each other in our structure
    WRITE_MEMORY(0x1405811EA, uint32_t, 0x1);

    // Redirect PV loader's existence checks to the DML map
    WRITE_CALL(originalPvLoaderGetPvDifficultyState, implOfPvLoaderGetPvDifficultyState);
    WRITE_NOP(reinterpret_cast<uint8_t*>(originalPvLoaderGetPvDifficultyState) + 0xC, 0xA);

    WRITE_JUMP(originalPvLoaderGetPvExists, implOfPvLoaderGetPvExists);

    // Scan the pv_db file before reading it to not waste time looking for entries that don't exist in the file
    WRITE_CALL(originalPvLoaderParseStart, implOfPvLoaderParseStart);
    WRITE_NOP(reinterpret_cast<uint8_t*>(originalPvLoaderParseStart) + 0xC, 0x3);
    
    WRITE_JUMP(originalPvLoaderParseLoop, implOfPvLoaderParseLoop);
}
