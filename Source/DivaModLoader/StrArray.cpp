#include "StrArray.h"

#include "Context.h"
#include "ModLoader.h"
#include "SigScan.h"
#include "Utilities.h"

typedef std::map<int, std::string> StrByIdMap;

static StrByIdMap strMap;
static StrByIdMap moduleStrMap;
static StrByIdMap customizeStrMap;

static void readStrArray(const toml::table* table, StrByIdMap& dstStrMap)
{
    if (!table)
        return;

    for (auto&& [key, value] : *table)
    {
        if (value.is_table())
            continue;

        // Convert to integer and check for success.
        char* end = nullptr;
        const int id = strtol(key.data(), &end, 10);

        if (end && dstStrMap.find(id) == dstStrMap.end())
            dstStrMap.insert({ id, value.value_or("YOU FORGOT QUOTATION MARKS") });
    }
}

static void readStrArray(const toml::table* table, const toml::table* langTable, StrByIdMap& dstStrMap)
{
    if (langTable != nullptr)
        readStrArray(langTable, dstStrMap);

    readStrArray(table, dstStrMap);
}

static void readStrArray(const toml::table* table, const toml::table* langTable, const char* name, StrByIdMap& dstStrMap)
{
    if (langTable != nullptr)
        readStrArray(langTable->get_as<toml::table>(name), dstStrMap);

    readStrArray(table->get_as<toml::table>(name), dstStrMap);
}

static void loadStrArray(const std::string& filePath)
{
    if (!std::filesystem::exists(filePath))
        return;

    toml::table table;

    try
    {
        table = toml::parse_file(filePath);
    }
    catch (std::exception& exception)
    {
        char text[0x400];
        sprintf(text, "Failed to parse \"%s\".\nDid you forget to add quotation marks to your string?\n\nDetails:\n%s",
            std::filesystem::path(filePath).lexically_normal().string().c_str(), exception.what());

        LOG("%s", text)
        MessageBoxA(nullptr, text, "DIVA Mod Loader", MB_ICONERROR);

        return;
    }

    uint8_t* instrAddr = (uint8_t*)sigLoadStrArray() + 0x55;
    FUNCTION_PTR(const char*, __fastcall, getLangDir, instrAddr + readUnalignedU32(instrAddr + 0x1) + 0x5);

    toml::table* langTable = table.get_as<toml::table>(strstr(getLangDir(), "/") + 1);

    readStrArray(&table, langTable, strMap);
    readStrArray(&table, langTable, "module", moduleStrMap);
    readStrArray(&table, langTable, "customize", customizeStrMap);
}

HOOK(void, __fastcall, LoadStrArray, sigLoadStrArray())
{
    originalLoadStrArray();

    for (auto& dir : ModLoader::modDirectoryPaths)
        loadStrArray(dir + "/rom/lang2/mod_str_array.toml");
}

// These functions aren't implemented here. See StrArrayImp.asm for details.
HOOK(const char*, __fastcall, GetStr, sigGetStr(), const int id);
HOOK(const char*, __fastcall, GetModuleName, sigGetModuleName(), const int id);
HOOK(const char*, __fastcall, GetCustomizeName, sigGetCustomizeName(), const int id);

const char* getStrImp(const int id)
{
    const auto str = strMap.find(id);

    if (str != strMap.end())
        return str->second.c_str();

    return originalGetStr(id);
}

const char* getModuleNameImp(const int id, const int moduleId)
{
    const auto str = moduleStrMap.find(moduleId);

    if (str != moduleStrMap.end())
        return str->second.c_str();

    return getStrImp(id);
}

const char* getCustomizeNameImp(const int id, const int customizeId)
{
    const auto str = customizeStrMap.find(customizeId);

    if (str != customizeStrMap.end())
        return str->second.c_str();

    return getStrImp(id);
}

void StrArray::init()
{
    INSTALL_HOOK(LoadStrArray);
    INSTALL_HOOK(GetStr);
    WRITE_CALL(originalGetModuleName, implOfGetModuleName);
    WRITE_CALL(originalGetCustomizeName, implOfGetCustomizeName);
}
