#include "SpriteLoader.h"

// - Replace 0xFFF mask with 0x7FFF
// - Replace 0x1000 flag with 0x8000
// - Replace 0xF000 mask with 0x8000
// - Make sprite database loader account for these changes
//
// This allows for 32K sprite sets to be loaded in the game.

extern void spriteLoaderFixupInfoInSprite();

void SpriteLoader::init()
{
    WRITE_MEMORY(0x14028F553, uint32_t, 0x7FFF);
    WRITE_MEMORY(0x1405B8FF2, uint32_t, 0x7FFF);
    WRITE_MEMORY(0x1405BB80C, uint32_t, 0x7FFF);
    WRITE_MEMORY(0x1405BB852, uint32_t, 0x7FFF);
    WRITE_MEMORY(0x1405BB89C, uint32_t, 0x7FFF);

    WRITE_MEMORY(0x1405B72EC, uint32_t, 0x80000000);
    WRITE_MEMORY(0x1405B72F2, uint32_t, 0x80000000);

    WRITE_MEMORY(0x1405B737A, uint32_t, 0x80000000);
    WRITE_MEMORY(0x1405B7381, uint32_t, 0x80000000);

    WRITE_MEMORY(0x1405B7403, uint32_t, 0x80000000);
    WRITE_MEMORY(0x1405B7408, uint32_t, 0x80000000);

    WRITE_MEMORY(0x1405B743A, uint32_t, 0x80000000);
    WRITE_MEMORY(0x1405B7440, uint32_t, 0x80000000);

    WRITE_MEMORY(0x1405BCB2E, uint8_t, 31);

    WRITE_CALL(0x1405BBF33, spriteLoaderFixupInfoInSprite);
    WRITE_NOP(0x1405BBF33 + 0xC, 1);
}
