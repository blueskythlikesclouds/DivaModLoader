#include "SaveData.h"

#include "Types.h"

struct Score
{
    int32_t pvId;
    INSERT_PADDING(0x132C);
};

static std::unordered_map<uint32_t, Score> scoreMap;

struct SaveDataEx
{
    static constexpr uint32_t MAX_VERSION = 0;
    static constexpr char FILE_NAME[] = "DivaModLoader.dat";

    uint32_t version;
    uint32_t headerSize;

    uint32_t scoreCount;
    // ...add more data in new versions as necessary

    Score* scores()
    {
        return reinterpret_cast<Score*>(reinterpret_cast<uint8_t*>(this) + headerSize);
    }

    // ...add more functions in new versions as necessary
};

static FUNCTION_PTR(void, __fastcall, getSaveDataFilePath, 0x1401D70D0, prj::string& dstFilePath, const prj::string& fileName);
static FUNCTION_PTR(void, __fastcall, getSaveDataKey, 0x1401D76F0, prj::string& dstKey, const prj::string& fileName, bool);

static FUNCTION_PTR(bool, __fastcall, readSaveData, 0x1401D7C90,
    const prj::string& fileName, prj::unique_ptr<uint8_t[]>& dst, size_t& dstSize);

static FUNCTION_PTR(bool, __fastcall, writeSaveData, 0x1401D79D0,
    const prj::string& key, const uint8_t* src, size_t srcSize, prj::unique_ptr<uint8_t[]>& dst, size_t& dstSize);

HOOK(void, __fastcall, LoadSaveData, 0x1401D7FB0, void* A1)
{
    originalLoadSaveData(A1);

    prj::unique_ptr<uint8_t[]> data;
    size_t dataSize = 0;

    if (!readSaveData(SaveDataEx::FILE_NAME, data, dataSize) || dataSize < sizeof(SaveDataEx))
        return;

    const auto saveData = reinterpret_cast<SaveDataEx*>(data.get());

    if (saveData->version > SaveDataEx::MAX_VERSION || saveData->headerSize > dataSize)
        return;

    for (uint32_t i = 0; i < saveData->scoreCount; i++)
    {
        auto& score = saveData->scores()[i];
        scoreMap.insert(std::make_pair(score.pvId, score));
    }
}

HOOK(void, __fastcall, SaveSaveData, 0x1401D8280, void* A1)
{
    originalSaveSaveData(A1);

    if (scoreMap.empty())
        return;

    std::vector<uint8_t> data(sizeof(SaveDataEx));

    const auto saveData = reinterpret_cast<SaveDataEx*>(data.data());
    saveData->version = SaveDataEx::MAX_VERSION;
    saveData->headerSize = sizeof(SaveDataEx);
    saveData->scoreCount = static_cast<uint32_t>(scoreMap.size());

    for (const auto& [pvId, score] : scoreMap)
    {
        const size_t offset = data.size();
        data.resize(offset + sizeof(Score));
        memcpy(&data[offset], &score, sizeof(Score));
    }

    prj::string filePath;
    prj::string key;

    getSaveDataFilePath(filePath, SaveDataEx::FILE_NAME);
    getSaveDataKey(key, SaveDataEx::FILE_NAME, true);

    prj::unique_ptr<uint8_t[]> fileData;
    size_t fileDataSize = 0;

    if (!writeSaveData(key, data.data(), data.size(), fileData, fileDataSize))
        return;

    FILE* file = fopen(filePath.c_str(), "wb");
    if (file != nullptr)
    {
        fwrite(fileData.get(), sizeof(uint8_t), fileDataSize, file);
        fclose(file);
    }
}

// See SaveDataImp.asm for implementations.
HOOK(Score*, __fastcall, FindOrCreateScore, 0x14E589750, void* A1, uint32_t pvId);
HOOK(Score*, __fastcall, FindScore, 0x14E5A32F0, void* A1, uint32_t pvId);

extern uint8_t EMPTY_SCORE_DATA[];

Score* findOrCreateScoreImp(void* A1, int32_t pvId)
{
    if (pvId >= 0)
    {
        const auto result = scoreMap.find(pvId);
        if (result != scoreMap.end())
            return &result->second;
    }

    Score* result = originalFindOrCreateScore(A1, pvId);

    if (result == nullptr && pvId >= 0)
    {
        result = &scoreMap[pvId];
        memcpy(result, EMPTY_SCORE_DATA, sizeof(Score));
        result->pvId = pvId;
    }

    return result;
}

Score* findScoreImp(void* A1, int32_t pvId)
{
    if (pvId >= 0)
    {
        const auto result = scoreMap.find(pvId);
        if (result != scoreMap.end())
            return &result->second;
    }

    return originalFindScore(A1, pvId);
}

void SaveData::init()
{
    INSTALL_HOOK(LoadSaveData);
    INSTALL_HOOK(SaveSaveData);
    INSTALL_HOOK(FindOrCreateScore);
    INSTALL_HOOK(FindScore);
}
