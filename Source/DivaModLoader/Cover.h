#pragma once
#include "Types.h"

class Cover
{
public:
    static int state;
    static void* fileHandler;
    
    static void init(const std::vector<std::string>& modRomDirectoryPaths);
    static bool resolveModFilePath(const prj::string& filePath, prj::string& destFilePath);
};