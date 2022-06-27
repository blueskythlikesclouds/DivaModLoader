#pragma once

typedef void CodeEvent();
typedef void OnFrameEvent(IDXGISwapChain* swapChain);

struct CodeEventPair
{
    std::wstring directoryPath;
    CodeEvent* event;

    void run() const;
};

class CodeLoader
{
public:
    static std::vector<std::wstring> dllFilePaths;

    static std::vector<CodeEventPair> initEvents;
    static std::vector<CodeEventPair> postInitEvents;
    static std::vector<OnFrameEvent*> onFrameEvents;

    static void init();
    static void postInit();
};