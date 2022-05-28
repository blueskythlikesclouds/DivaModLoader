#include "Context.h"
#include "Types.h"
#include "Utilities.h"

std::vector<std::string> modDirectoryPaths;

HOOK(void, __fastcall, InitRomDirectoryPaths, 0x1402A2040)
{
    originalInitRomDirectoryPaths();

    const auto romDirectoryPaths = (prj::vector<prj::string>*)0x1414B2768;

    std::vector<std::string> modRomDirectoryPaths;

    for (auto& modDirectoryPath : modDirectoryPaths)
    {
        for (auto& romDirectoryPath : *romDirectoryPaths)
            modRomDirectoryPaths.push_back(modDirectoryPath + "\\" + romDirectoryPath.c_str());
    }

    processDirectoryPaths(modRomDirectoryPaths, false);

    if (modRomDirectoryPaths.empty())
        return;

    LOG("ROM:")

    for (auto& modRomDirectoryPath : modRomDirectoryPaths)
        LOG(" - %s", modRomDirectoryPath.c_str());

    romDirectoryPaths->insert(romDirectoryPaths->begin(), modRomDirectoryPaths.begin(), modRomDirectoryPaths.end());
}

void initModLoader()
{
    LOG("Mods: \"%s\"", getRelativePath(modsDirectoryPath).c_str())

    for (auto& modDirectory : std::filesystem::directory_iterator(modsDirectoryPath))
    {
        if (!std::filesystem::is_directory(modDirectory))
            continue;

        const std::string modDirectoryPath = modDirectory.path().string();
        const std::string configFilePath = modDirectoryPath + "\\config.toml";

        toml::table config;

        try
        {
            config = toml::parse_file(configFilePath);
        }

        catch (std::exception& exception)
        {
            LOG(" - Failed to load \"%s\": %s", getRelativePath(configFilePath).c_str(), exception.what())
            continue;
        }

        if (!config["enabled"].value_or(true))
            continue;

        LOG(" - %s", config["name"].value_or(modDirectory.path().filename().string().c_str()))

        if (toml::array* includeArr = config["include"].as_array())
        {
            for (auto& includeElem : *includeArr)
            {
                const std::string include = includeElem.value_or("");

                if (!include.empty())
                    modDirectoryPaths.push_back(modDirectoryPath + "\\" + include);
            }
        }

        if (toml::array* dllArr = config["dll"].as_array())
        {
            for (auto& dllElem : *dllArr)
            {
                const std::string dll = dllElem.value_or("");

                if (!dll.empty())
                    dllFilePaths.push_back(modDirectory.path().wstring() + L"\\" + convertMultiByteToWideChar(dll));
            }
        }
    }

    INSTALL_HOOK(InitRomDirectoryPaths);
}