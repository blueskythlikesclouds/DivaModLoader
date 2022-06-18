#pragma once

typedef void CodeEvent();

class CodeLoader
{
public:
    static std::vector<std::wstring> dllFilePaths;

    static std::vector<CodeEvent*> initEvents;
    static std::vector<CodeEvent*> postInitEvents;
    static std::vector<CodeEvent*> onFrameEvents;

    static void init();
    static void postInit();
};