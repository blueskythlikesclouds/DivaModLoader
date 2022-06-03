#pragma once

class Config
{
public:
    static bool enableDebugConsole;
    static std::string modsDirectoryPath;
    static std::vector<std::string> priorityPaths;

    static bool init();
};
