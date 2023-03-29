#include "DatabaseLoader.h"

#include "Context.h"
#include "ModLoader.h"
#include "Types.h"
#include "Utilities.h"

// The game contains a list of database prefixes in the mount data manager.
// We insert all mod directory paths into this list along with a magic value wrapping it.

// For example, object database becomes "rom/objset/<magic><mod path><magic>_obj_db.bin".
// We detect this pattern in the file resolver function and fix it to become a valid file path.
// It becomes "<mod path>/rom/objset/mod_obj_db.bin" as a result.

constexpr char MAGIC = 0x01;

void resolveModFilePath(prj::string& filePath)
{
    const size_t magicIdx0 = filePath.find(MAGIC);
    if (magicIdx0 == std::string::npos)
        return;

    const size_t magicIdx1 = filePath.find(MAGIC, magicIdx0 + 1);
    if (magicIdx1 == std::string::npos)
        return;

    const prj::string left = filePath.substr(0, magicIdx0); // folder
    const prj::string center = filePath.substr(magicIdx0 + 1, magicIdx1 - magicIdx0 - 1); // mod folder
    const prj::string right = filePath.substr(magicIdx1 + 1); // file name

    filePath = center;
    filePath += "/";
    filePath += left;
    filePath += "mod";
    filePath += right;
}

HOOK(size_t, __fastcall, ResolveFilePath, sigResolveFilePath(), prj::string& filePath, prj::string* a2)
{
    resolveModFilePath(filePath);

    // I don't know what this is. It always seems to be the same as the input file path.
    if (a2)
        resolveModFilePath(*a2);

    return originalResolveFilePath(filePath, a2);
}

// Custom string arrays.
std::map<int, std::string> strArray;
std::map<int, std::string> moduleNames;

void addStrArray(const toml::table* table, std::map<int, std::string> *strArray)
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

        if (end && strArray->find(id) == strArray->end())
            strArray->insert({ id, value.value_or("YOU FORGOT QUOTATION MARKS") });
    }
}

void loadStrArray(const std::string& filePath)
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

    toml::table *langTable = table.get_as<toml::table>(strstr(getLangDir(), "/") + 1);
    addStrArray(langTable, &strArray);
    addStrArray(&table, &strArray);

    addStrArray(langTable->get_as<toml::table>("module"), &moduleNames);
    addStrArray(table.get_as<toml::table>("module"), &moduleNames);
}

HOOK(void, __fastcall, LoadStrArray, sigLoadStrArray())
{
    originalLoadStrArray();

    for (auto& dir : ModLoader::modDirectoryPaths)
        loadStrArray(dir + "/rom/lang2/mod_str_array.toml");
}

// This function isn't implemented here. See DatabaseLoaderImp.asm for details.
HOOK(const char*, __fastcall, GetStr, sigGetStr(), const int id);
extern "C" FUNCTION_PTR(void, __fastcall, GetModuleNameJumpBack, sigGetModuleName() + readUnalignedU32(sigGetModuleName() + 1) + 5 + 0x4E0 + 5);
HOOK(const char*, __fastcall, GetModuleName, sigGetModuleName() + readUnalignedU32(sigGetModuleName() + 1) + 5 + 0x4E0, const int id);

const char* getStrImp(const int id)
{
    const auto str = strArray.find(id);

    if (str != strArray.end())
        return str->second.c_str();

    return originalGetStr(id);
}

const char* getModuleNameImp(const int id)
{
    const int moduleId = id - 0x95;
    const auto str = moduleNames.find(moduleId);

    if (str != moduleNames.end())
        return str->second.c_str();

    return getStrImp(id);
}

void DatabaseLoader::init()
{
    INSTALL_HOOK(ResolveFilePath);

    // Safe to do this as this list is initialized in a C++ static
    // initializer function which gets called before WinMain.

    // Get the list address from the lea instruction that loads it.
    uint8_t* instrAddr = (uint8_t*)sigInitMdataMgr() + 0xFE;
    auto& list = *(prj::list<prj::string>*)(instrAddr + readUnalignedU32(instrAddr + 0x3) + 0x7);

    // Traverse mod folders in reverse to have correct priority.
    for (auto it = ModLoader::modDirectoryPaths.rbegin(); it != ModLoader::modDirectoryPaths.rend(); ++it)
    {
        prj::string path;

        path += MAGIC;
        path += *it;
        path += MAGIC;
        path += "_";

        list.push_back(path);
    }

    INSTALL_HOOK(LoadStrArray);
    INSTALL_HOOK(GetStr);
}