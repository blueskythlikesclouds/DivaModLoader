#pragma once

class Config
{
public:
    static bool enableDebugConsole;
    static std::string modsDirectoryPath;

    static bool init();
};
