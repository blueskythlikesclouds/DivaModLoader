#include "SaveData.h"

struct Score
{
    int32_t pvId;
    INSERT_PADDING(0x132C);
};

static std::unordered_map<uint32_t, Score> scoreMap;

struct SaveDataEx
{
    static constexpr uint32_t MAX_VERSION = 0;

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

struct SaveDataExContainer
{
    uint32_t compressedSize;
    uint32_t decompressedSize;
};

HOOK(void, __fastcall, LoadSaveData, 0x1401D7FB0, void* A1)
{
    originalLoadSaveData(A1);

    FILE* file = fopen("DivaModLoader.dat", "rb");
    if (file == nullptr)
        return;

    SaveDataExContainer container{};
    fread(&container, sizeof(SaveDataExContainer), 1, file);

    const auto compressedData = std::make_unique<uint8_t[]>(container.compressedSize);

    fread(compressedData.get(), sizeof(uint8_t), container.compressedSize, file);
    fclose(file);

    const auto decompressedData = std::make_unique<uint8_t[]>(container.decompressedSize);
    uLongf decompressedSize = container.decompressedSize;

    if (uncompress(decompressedData.get(), &decompressedSize, compressedData.get(), container.compressedSize) != Z_OK)
        return;

    const auto saveData = reinterpret_cast<SaveDataEx*>(decompressedData.get());

    if (saveData->version > SaveDataEx::MAX_VERSION || saveData->headerSize > decompressedSize)
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

    uLongf compressedSize = compressBound(static_cast<uint32_t>(data.size()));
    const auto compressedData = std::make_unique<uint8_t[]>(compressedSize);

    if (compress(compressedData.get(), &compressedSize, data.data(), static_cast<uLong>(data.size())) != Z_OK)
        return;

    FILE* file = fopen("DivaModLoader.dat", "wb");
    if (file != nullptr)
    {
        const SaveDataExContainer container = 
        {
            compressedSize, // compressedSize
            static_cast<uint32_t>(data.size()) // decompressedSize
        };

        fwrite(&container, sizeof(SaveDataExContainer), 1, file);
        fwrite(compressedData.get(), sizeof(uint8_t), compressedSize, file);
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
