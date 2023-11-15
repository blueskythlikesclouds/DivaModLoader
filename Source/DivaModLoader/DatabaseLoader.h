#pragma once

class DatabaseLoader
{
public:
    static void init();
    static void initMdataMgr(const std::vector<std::string>& modRomDirectoryPaths);
};
