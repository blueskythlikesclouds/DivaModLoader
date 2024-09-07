# DIVA Mod Loader

DIVA Mod Loader (DML) is a mod loader for Hatsune Miku: Project DIVA Mega Mix+. It allows you to make file replacement mods without hassle and provides extra features for convenience.

You can use [DIVA Mod Manager (DMM)](https://github.com/TekkaGB/DivaModManager) or [d4m](https://github.com/Brod8362/d4m) as a front-end for DML.

## Installation

* Download the latest version from the [Releases](https://github.com/blueskythlikesclouds/DivaModLoader/releases) page. Or, if you prefer, compiling from source with the instructions far below.
* Extract everything to where **DivaMegaMix.exe** is located.
* **dinput8.dll**, **config.toml** and **mods** must be next to this executable file.
* Linux users (including Steam Deck) additionally need to do the following:
    1. Right click the game on Steam.
    2. Select Properties -> Launch Options.
    3. Enter `WINEDLLOVERRIDES='dinput8.dll=n,b' %command%` as the command.
* MacOS users (using Apple's Game Port Toolkit) additionaly need to do the following:
    1. Open winecfg (in Terminal write `gameportingtoolkit ~/change-to-your-prefix-name winecfg`)  
    2. Select Libraries
    3. Open the dropdown menu -> select from the list `dinput8.dll` -> press Add
    4. Make sure it says `dinput8(native, builtin)`. if not, press Edit and select `Native and Builtin`.

Some mods may require specific versions of the Visual C++ Redistributable to be installed on your system, even though DIVA Mod Loader itself doesn't need them. To ensure full compatibility with all mods, please install [Visual C++ Redistributable Runtimes All-in-One](https://www.techpowerup.com/download/visual-c-redistributable-runtime-package-all-in-one/).

## Features

### Configuration File

**config.toml** located next to **dinput8.dll** allows you to configure the behavior of DML.

```toml
enabled = true
console = false
mods = "mods"
priority = ["Example Mod 1", "Example Mod 2"]
```

* **enabled**: Whether the mod loader is enabled.  
* **console**: Whether a console window is going to be created.  
* **mods**: The directory where mods are stored.  
* **priority**: A list of mod folders to load, with the first mod in the array having the highest priority.

The priority array is automatically set by mod managers. If you're not using a mod manager, you can delete the priority array from the config file. This will make mods load in alphabetical order from the mods folder, with the mod at the top of the list having the highest priority. This is the default behavior if you have installed DML directly from the GitHub page without a mod manager.

### Mod Loading

Each mod contains a **config.toml** file located within the mod directory.

```toml
enabled = true
include = ["."]
dll = ["TemplateMod.dll"]

name = "Template Mod"
description = "This is a template mod!"
version = "1.0"
date = "27.05.2022"
author = "Skyth"
```

* **enabled**: Whether the mod is going to be loaded.  
* **include**: A list of directories to scan when doing external file replacements. Most mods only need `"."` as the value. If your mod only contains a DLL file without replacing any files, you should remove this property.  
* **dll**: A list of DLL files to load. You should remove this property if your mod doesn't utilize any DLL files, otherwise you are going to receive errors at startup.  

Rest of the properties should be self-explanatory and are not used by DML. They are only used by DMM or d4m to display information about the mod if it wasn't downloaded from GameBanana. However, please make sure to use version/date values in the same format as the given example.

### File Replacement

An example mod file structure is as follows:

* mods
  * Example Mod
    * config.toml
    * rom
      * objset
        * mikitm001.farc
      * lang2
        * mod_str_array.toml
    * rom_steam_en
      * rom
        * 2d
          * spr_gam_cmn.farc

As you can see, the files are organized in a way that is similar to the CPKs. However, most of the time, you only need to use the main **rom** directory. Rest of the **rom** directories are used to replace files for specific languages.

### Mod Database Loading

DML can load mod databases which contain entries only relevant to the mod. This makes it possible for multiple mods to co-exist without conflicts. For example, when adding a new song, you can add its PV entry to **mod_pv_db.txt** file without having to include entries from the base game or other mods.

This works with all database types, **mod_obj_db.bin**, **mod_tex_db.bin**, **mod_aet_db.bin**, etc. However, please note that ID conflicts are still an issue when using this method. Automatic ID conflict fixing is planned to be implemented in the future.

### Mod String Array Loading

Mods cannot replace **str_array.bin** files without overriding each other. As a solution, DML can load **mod_str_array.toml** files located in the **lang2** directory with entries only relevant to the mod.

An example string array file from the [Mikudayo/Mikunano mod](https://gamebanana.com/mods/384277) is as follows:

```toml
1273 = "Mikudayo"
cn.1273 = "MIKUDAYO"
kr.1273 = "미쿠다요"
tw.1273 = "MIKUDAYO"
1274 = "Mikunano"
cn.1274 = "MIKUNANO"
kr.1274 = "미쿠나노"
tw.1274 = "MIKUNANO"
```

An `id = "value"` pair declares a string for all languages. If you want to declare a string for a specific language, you need to prefix the id with the language code. For example, `cn.1273 = "MIKUDAYO"` declares the string for Chinese. Language specific strings take precedence over the global ones.

Please make sure all strings are wrapped in quotes.

#### Grouped String Arrays

When following the original string array file layout, a few limitations arise. For instance, since modules have their specific range starting from 275, adding too many module names outside the range could override customize item names. To solve this issue, DML provides a feature where you can specify strings in separate named arrays by their original ID, without worrying about accidentally overriding other strings.

To demonstrate this format, the previous string array example can be redone as follows:

```toml
module.998 = "Mikudayo"
cn.module.998 = "MIKUDAYO"
kr.module.998 = "미쿠다요"
tw.module.998 = "MIKUDAYO"
module.999 = "Mikunano"
cn.module.999 = "MIKUNANO"
kr.module.999 = "미쿠나노"
tw.module.999 = "MIKUNANO"
```

In this example, 998/999 numbers correspond to the module IDs, instead of the 1273/1274 in the previous example where the required offset number 275 was added to them. **Modders are advised to exclusively use this format.** 

All of the following string types are supported, and the same ID rule applies to all of them:

* module
* customize
* cstm_item
* btn_se
* slide_se
* chainslide_se
* slidertouch_se

Please note that both formats can be used at the same time in the same file.

### DLL Loading

DML allows you to inject your own code into the game by loading DLL files. Certain functions can be exposed in the DLLs for initialization and per frame updates.

```cpp
extern "C"
{
    void __declspec(dllexport) PreInit() {}

    void __declspec(dllexport) Init() {}

    void __declspec(dllexport) PostInit() {}

    void __declspec(dllexport) D3DInit(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* deviceContext) {}

    void __declspec(dllexport) OnFrame(IDXGISwapChain* swapChain) {}	

    void __declspec(dllexport) OnResize(IDXGISwapChain* swapChain) {}
}
```

* **PreInit** is called before any of the game's global variables are initialized. You only need to use this if you want to hook into C++ static initializers.  
* **Init** is called after the game's global variables are initialized. This is likely going to be the function where you initialize your mod and inject code. You cannot hook into C++ static initializers from this function as they have been called already.  
* **PostInit** is called after every DLL mod is initialized. This is a pretty niche use case and you probably won't need it.
* **D3DInit** is called in **D3D11CreateDeviceAndSwapChain** after the device gets initialized. You could use this (along with **OnFrame**) to hook ImGui into the game.
* **OnFrame** is called before the game's frame is presented to the screen. You can use this to execute logic per frame.  
* **OnResize** is called after the game's buffers are resized.

You can omit functions that you don't need to use. They are not required to be declared.

You can use different naming conventions for your functions, eg. **PreInit**, **preInit** and **pre_init**.

The current directory is changed to where the DLL is located before any of the **Init** functions are called. This is useful if you want to use relative paths.

Please refrain from executing any logic in **DllMain**, use these exported functions instead. **DllMain** has too many limitations and issues to be considered reliable.

It’s recommended to statically link the runtime in your DLL. In Visual Studio, you can do this by setting `Configuration Properties -> C/C++ -> Code Generation -> Runtime Library` to `Multi-threaded (/MT)` for the release configuration and `Multi-threaded Debug (/MTd)` for the debug configuration. This ensures that your mod will load properly even if the user doesn't have the required runtime installed on their system.

## Compiling

In order to build from source, the following prerequisite software must be installed:
* **Visual Studio 2022** with the **Desktop Development with C++** package installed, **Git for Windows**, and **CMake Tools**.

Now, clone the repository, and enter that directory.
You can do this via **Git Bash**, which is installed by the **Git for Windows** package from the **Visual Studio Installer**.
```
git clone https://github.com/blueskythlikesclouds/DivaModLoader.git
cd DivaModLoader
```

Take note of the current directory you are residing in.
Then, open a **Developer Command Prompt** for Visual Studio by pressing your Windows (or SUPER) key and searching.
You want either the command prompt designated for **Native x64**, or the generic one.
Change directories into that repository that you cloned earlier...
```
cd PATH_TO_DIVAMODLOADER_REPO
```

Now... run the following command to configure the CMake project:
```
cmake -B build -A x64 -DCMAKE_BUILD_TYPE=Release -DFETCH_THIRDPARTY_DEPS=ON
```
Then, once configured, build the project like so:
```
cmake --build build --config Release
```

The mod loader, `dinput8.dll` will now be located within your relative path at `build\Release\dinput8.dll`.
Enjoy.
