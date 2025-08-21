#include "SpriteLoader.h"

#include "SigScan.h"

// - Replace 0xFFF mask with 0x7FFF
// - Replace 0x1000 flag with 0x8000
// - Replace 0xF000 mask with 0x8000
// - Make sprite database loader account for these changes
//
// This allows for 32K sprite sets to be loaded in the game.

SIG_SCAN
(
    sigSpriteMask1,
    0x14028F551,
    "\x81\xE2\xFF\x0F\x00\x00\xE8\xCC\xCC\xCC\xCC\x8B\x08", 
    "xxxxxxx????xx"
);

SIG_SCAN
(
    sigSpriteMask2,
    0x1405B8FF0,
    "\x81\xE2\xFF\x0F\x00\x00\xE8\xCC\xCC\xCC\xCC\x48\x8B\xE8", 
    "xxxxxxx????xxx"
);

SIG_SCAN
(
    sigSpriteMask3,
    0x1405BB80A,
    "\x81\xE2\xFF\x0F\x00\x00\xE8\xCC\xCC\xCC\xCC\x48\x85\xC0\x74\x0F\x41\x8B\xD1\x48\x8B\xC8\x48\x83\xC4\x28\xE9\xCC\xCC\xCC\xCC\x48\x8D\x05", 
    "xxxxxxx????xxxxxxxxxxxxxxxx????xxx"
);

SIG_SCAN
(
    sigSpriteMask4,
    0x1405BB850,
    "\x81\xE2\xFF\x0F\x00\x00\xE8\xCC\xCC\xCC\xCC\x48\x85\xC0\x74\x16", 
    "xxxxxxx????xxxxx"
);

SIG_SCAN
(
    sigSpriteMask5,
    0x1405BB89A,
    "\x81\xE2\xFF\x0F\x00\x00\xE8\xCC\xCC\xCC\xCC\x48\x85\xC0\x74\x0F\x41\x8B\xD1\x48\x8B\xC8\x48\x83\xC4\x28\xE9\xCC\xCC\xCC\xCC\x48\x83\xC4\x28", 
    "xxxxxxx????xxxxxxxxxxxxxxxx????xxxx"
);

SIG_SCAN
(
    sigSpriteFlag1,
    0x1405B72EA,
    "\x81\xE2\x00\x00\x00\xF0\x81\xFA\x00\x00\x00\x10\x75\x0A", 
    "xxxxxxxxxxxxxx"
);

SIG_SCAN
(
    sigSpriteFlag2,
    0x1405B7377,
    "\x41\x81\xE0\x00\x00\x00\xF0\x41\x81\xF8\x00\x00\x00\x10", 
    "xxxxxxxxxxxxxx"
);

SIG_SCAN
(
    sigSpriteFlag3,
    0x1405B7402,
    "\x25\x00\x00\x00\xF0\x3D\x00\x00\x00\x10", 
    "xxxxxxxxxx"
);

SIG_SCAN
(
    sigSpriteFlag4,
    0x1405B7438,
    "\x81\xE2\x00\x00\x00\xF0\x81\xFA\x00\x00\x00\x10\x75\x09", 
    "xxxxxxxxxxxxxx"
);

SIG_SCAN
(
    sigSpriteFlag5,
    0x1405BCB2B,
    "\x0F\xBA\xED\x1C\xE8", 
    "xxxxx"
);

SIG_SCAN
(
    sigSpriteFlag6,
    0x1405B7693,
    "\x0F\xBA\xE8\x0C\xC1\xE0\x10",
    "xxxxxxx"
);

SIG_SCAN
(
    sigSpriteFlagFixup,
    0x1405BBF33,
    "\x4A\x89\x4C\x1A\x08\x41\x8B\x00\x42\x89\x44\x1A\x10\xE8\xCC\xCC\xCC\xCC\x44\x03\xC8", 
    "xxxxxxxxxxxxxx????xxx"
)

extern void spriteLoaderFixupInfoInSprite();

void SpriteLoader::init()
{
    WRITE_MEMORY((uint8_t*)sigSpriteMask1() + 0x2, uint32_t, 0x7FFF);
    WRITE_MEMORY((uint8_t*)sigSpriteMask2() + 0x2, uint32_t, 0x7FFF);
    WRITE_MEMORY((uint8_t*)sigSpriteMask3() + 0x2, uint32_t, 0x7FFF);
    WRITE_MEMORY((uint8_t*)sigSpriteMask4() + 0x2, uint32_t, 0x7FFF);
    WRITE_MEMORY((uint8_t*)sigSpriteMask5() + 0x2, uint32_t, 0x7FFF);

    WRITE_MEMORY((uint8_t*)sigSpriteFlag1() + 0x2, uint32_t, 0x80000000);
    WRITE_MEMORY((uint8_t*)sigSpriteFlag1() + 0x8, uint32_t, 0x80000000);

    WRITE_MEMORY((uint8_t*)sigSpriteFlag2() + 0x3, uint32_t, 0x80000000);
    WRITE_MEMORY((uint8_t*)sigSpriteFlag2() + 0xA, uint32_t, 0x80000000);

    WRITE_MEMORY((uint8_t*)sigSpriteFlag3() + 0x1, uint32_t, 0x80000000);
    WRITE_MEMORY((uint8_t*)sigSpriteFlag3() + 0x6, uint32_t, 0x80000000);

    WRITE_MEMORY((uint8_t*)sigSpriteFlag4() + 0x2, uint32_t, 0x80000000);
    WRITE_MEMORY((uint8_t*)sigSpriteFlag4() + 0x8, uint32_t, 0x80000000);

    WRITE_MEMORY((uint8_t*)sigSpriteFlag5() + 0x3, uint8_t, 31);

    WRITE_MEMORY((uint8_t*)sigSpriteFlag6() + 0x3, uint8_t, 0xF);

    WRITE_CALL(sigSpriteFlagFixup(), spriteLoaderFixupInfoInSprite);
    WRITE_NOP((uint8_t*)sigSpriteFlagFixup() + 0xC, 1);
}
