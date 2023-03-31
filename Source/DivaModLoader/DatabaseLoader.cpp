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
}