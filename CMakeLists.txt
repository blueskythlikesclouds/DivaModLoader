cmake_minimum_required(VERSION 3.28.0)

option(FETCH_THIRDPARTY_DEPS "Fetch third-party dependencies (toml++, Detours) instead of using repository-provided renditions, ensuring they are compiled from source." OFF)

# C++ 17, project uses C++ and MASM
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(DIVAMODLOADER_ROOT "${CMAKE_CURRENT_LIST_DIR}")
project(DivaModLoader
  LANGUAGES
    CXX ASM_MASM)

# Mod loader is built as a DLL
add_library(DivaModLoader SHARED)

# Mod loader shall be compiled, and henceforth named: "dinput8.dll"
# This is so that it gets statically imported upon executable load
set_target_properties(DivaModLoader PROPERTIES OUTPUT_NAME "dinput8")

# Source files associated with the mod loader...
set(DIVAMODLOADER_SRC_ROOT "${DIVAMODLOADER_ROOT}/Source/DivaModLoader")
set(DIVAMODLOADER_SRC "${DIVAMODLOADER_SRC_ROOT}/Allocator.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/CodeLoader.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/Config.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/Context.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/DatabaseLoader.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/FileLoader.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/ModLoader.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/Patches.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/Pch.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/PvLoader.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/PvLoaderImp.asm"
                      "${DIVAMODLOADER_SRC_ROOT}/SaveData.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/SaveDataImp.asm"
                      "${DIVAMODLOADER_SRC_ROOT}/ScoreData.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/SpriteLoader.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/SpriteLoaderImp.asm"
                      "${DIVAMODLOADER_SRC_ROOT}/StrArray.cpp"
                      "${DIVAMODLOADER_SRC_ROOT}/StrArrayImp.asm")

# Add the designated source files to DivaModLoader
target_sources(DivaModLoader PRIVATE "${DIVAMODLOADER_SRC}")

# Some thoughts before continuing...
# Any internal dependencies can be in an Include directory, whereas third party submoduled dependencies would be in the aptly named ThirdParty folder
# Or actually, if a user wants to use thirdparty sourced dependencies rather than the ones hosted in this very repository, FetchContent will be used... yeah, that sounds a lot better.

# First-party include
target_include_directories(DivaModLoader PRIVATE "${DIVAMODLOADER_ROOT}/Include")

if(NOT FETCH_THIRDPARTY_DEPS)
  # Use repository provided dependencies...
  target_include_directories(DivaModLoader PRIVATE "${DIVAMODLOADER_ROOT}/Dependencies"
                                                    "${DIVAMODLOADER_ROOT}/Dependencies/Detours/include")
  target_link_libraries(DivaModLoader PRIVATE "${DIVAMODLOADER_ROOT}/Dependencies/Detours/lib/detours.lib"
                                              "${DIVAMODLOADER_ROOT}/Dependencies/Detours/lib/syelog.lib")
else()
  # Fetch third-party dependencies from their respective repositories
  include(FetchContent)

  # DivaModLoader uses v3.1.0 (current live stable ver. is v3.4.0)
  FetchContent_Declare(
      tomlplusplus
      GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
      GIT_TAG        v3.1.0
  )
  FetchContent_MakeAvailable(tomlplusplus)
  target_link_libraries(DivaModLoader PRIVATE tomlplusplus::tomlplusplus)
endif()
