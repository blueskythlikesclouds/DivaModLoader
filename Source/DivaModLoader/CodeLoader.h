#pragma once

typedef void InitEvent();
typedef void D3DInitEvent(IDXGISwapChain*, ID3D11Device*, ID3D11DeviceContext*);
typedef void OnFrameEvent(IDXGISwapChain*);

template<typename T>
struct EventPair
{
    std::wstring directoryPath;
    T* event;

    template<typename... Args>
    void run(Args... args) const
    {
        SetCurrentDirectoryW(directoryPath.c_str());
        SetDllDirectoryW(directoryPath.c_str());

        event(args...);
    }
};

class CodeLoader
{
public:
    static std::vector<std::wstring> dllFilePaths;

    static std::vector<EventPair<InitEvent>> initEvents;
    static std::vector<EventPair<InitEvent>> postInitEvents;
    static std::vector<EventPair<D3DInitEvent>> d3dInitEvents;
    static std::vector<OnFrameEvent*> onFrameEvents;
    static std::vector<OnFrameEvent*> onResizeEvents;

    static void init();
    static void postInit();
};