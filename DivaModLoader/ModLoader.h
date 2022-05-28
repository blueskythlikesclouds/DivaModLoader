#pragma once

class ModLoader
{
public:
    static std::vector<std::string> modDirectoryPaths;

    static void initMod(const std::filesystem::path& path);
    static void init();
};