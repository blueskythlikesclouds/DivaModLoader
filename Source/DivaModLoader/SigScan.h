#pragma once

extern bool sigValid;

extern void* sigWinMain(); // 0x1402C2F30

extern void* sigOperatorNew(); // 0x14097DEC0
extern void* sigOperatorDelete(); // 0x1409B8580

extern void* sigInitRomDirectoryPaths(); // 0x1402A2040

extern void* sigInitSteamAPIManager(); // 0x1406051D0

extern void* sigResolveFilePath(); // 0x1402A5030
extern void* sigInitMdataMgr(); // 0x140442D50