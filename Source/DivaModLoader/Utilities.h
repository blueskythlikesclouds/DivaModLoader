#pragma once

/// Removes directories that don't exist, and tries to make their paths relative if they are contained within the current directory.
inline void processDirectoryPaths(std::vector<std::string>& directoryPaths, const bool reverse)
{
    std::vector<std::string> newDirectoryPaths;

    const std::filesystem::path currentPath = std::filesystem::current_path().lexically_normal();

    for (auto& directoryPath : directoryPaths)
    {
        const std::filesystem::path path = std::filesystem::path(directoryPath).lexically_normal();

        if (!std::filesystem::is_directory(path))
            continue;

        const std::string newDirectoryPath = path.string();
        const std::string relativePath = std::filesystem::relative(path, currentPath).string();

        std::string newPath = !relativePath.empty() && relativePath.size() < newDirectoryPath.size() ? relativePath : newDirectoryPath;

        std::replace(newPath.begin(), newPath.end(), '\\', '/');

        if (!newPath.empty() && newPath.back() == '/')
            newPath.pop_back();

        newDirectoryPaths.push_back(newPath);
    }

    if (reverse)
        std::reverse(newDirectoryPaths.begin(), newDirectoryPaths.end());

    std::swap(directoryPaths, newDirectoryPaths);
}

/// Removes files that don't exist, and tries to make their paths relative if they are contained within the current directory.
inline void processFilePaths(std::vector<std::wstring>& filePaths, const bool reverse)
{
    std::vector<std::wstring> newFilePaths;

    for (auto& filePath : filePaths)
    {
        if (!std::filesystem::is_regular_file(filePath))
            continue;

        std::wstring newPath = std::filesystem::absolute(filePath).lexically_normal().wstring();
        std::replace(newPath.begin(), newPath.end(), L'\\', L'/');

        newFilePaths.push_back(newPath);
    }

    if (reverse)
        std::reverse(newFilePaths.begin(), newFilePaths.end());

    std::swap(filePaths, newFilePaths);
}

inline std::wstring convertMultiByteToWideChar(const std::string& value)
{
    WCHAR wideChar[0x400];
    MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, wideChar, _countof(wideChar));

    return std::wstring(wideChar);
}

/// Gets relative path if it's contained within the current directory.
inline std::string getRelativePath(const std::string& filePath)
{
    std::string relativePath = std::filesystem::relative(filePath, std::filesystem::current_path()).string();

    if (relativePath.empty() || relativePath.size() > filePath.size())
        relativePath = filePath;

    std::replace(relativePath.begin(), relativePath.end(), '\\', '/');

    return relativePath;
}

/// Gets relative path if it's contained within the current directory.
inline std::wstring getRelativePath(const std::wstring& filePath)
{
    std::wstring relativePath = std::filesystem::relative(filePath, std::filesystem::current_path()).wstring();

    if (relativePath.empty() || relativePath.size() > filePath.size())
        relativePath = filePath;

    std::replace(relativePath.begin(), relativePath.end(), L'\\', L'/');

    return relativePath;
}

inline void* readInstrPtr(void* function, ptrdiff_t instrOffset, ptrdiff_t instrSize)
{
    uint8_t* instrAddr = (uint8_t*)function + instrOffset;
    return instrAddr + *(int32_t*)(instrAddr + instrSize - 0x4) + instrSize;
}