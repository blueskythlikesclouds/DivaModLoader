#pragma once

#define LOG(x, ...) \
    { \
        if (enableDebugConsole) \
        { \
            printf(""##x##"\n", __VA_ARGS__); \
        } \
    }

extern bool enableDebugConsole;
extern std::string modsDirectoryPath;

extern std::vector<std::string> modDirectoryPaths;
extern std::vector<std::wstring> dllFilePaths;

extern void initSteam();
extern void initModLoader();
extern void initCodeLoader();

extern void init();