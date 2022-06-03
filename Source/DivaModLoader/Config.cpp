#include "Config.h"

bool Config::enableDebugConsole;
std::string Config::modsDirectoryPath;
std::vector<std::string> Config::priorityPaths;

bool Config::init()
{
    toml::table config;
    try
    {
        config = toml::parse_file("config.toml");
    }

    catch (std::exception& exception)
    {
        char text[1024];
        sprintf(text, "Failed to parse config.toml:\n%s", exception.what());
        MessageBoxA(nullptr, text, "DIVA Mod Loader", MB_OK | MB_ICONERROR);
    }

    if (!config["enabled"].value_or(true))
        return false;

    enableDebugConsole = config["console"].value_or(false);
    modsDirectoryPath = config["mods"].value_or("mods");

    if (toml::array* priorityArr = config["priority"].as_array())
    {
        for (auto& pathElem : *priorityArr)
        {
            const std::string path = pathElem.value_or("");
            if (!path.empty())
                priorityPaths.push_back(path);
        }
    }

    return true;
}
