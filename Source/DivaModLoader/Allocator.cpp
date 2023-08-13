#include "Allocator.h"
#include "SigScan.h"

SIG_SCAN
(
    sigOperatorNew,
    0x14014B889,
    "\xE8\xCC\xCC\xCC\xCC\x4C\x8D\x45\x01", 
    "x????xxxx"
); // call to function, E8 ?? ?? ?? ??

SIG_SCAN
(
    sigOperatorDelete,
    0x1409B1E90,
    "\x48\x85\xC9\x74\x37\x53\x48\x83\xEC\x20\x4C\x8B", 
    "xxxxxxxxxxxx"
);

SIG_SCAN
(
    sigHeapCMallocAllocate,
    0x1404402B0,
    "\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x48\x83\xEC\x20\x0F\xB6\x05", 
    "xxxxxxxxxxxxxxxxxx"
);