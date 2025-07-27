#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Constants.h"
#include "Interpreter/Hardware/RAM.h"

// System
#include <array>
#include <cassert>
#include <algorithm>

//--------------------------------------------------------------------------------
class Display
{
#ifdef UNIT_TESTING
    friend class DisplayTestAccessor;
#endif

public:
    Display()
        : mBuffer{ }
    { }

    void SetRAM(RAM& ram) { mRAM = &ram; }

    [[nodiscard]] uint8_t DrawSprite(uint32_t px, uint32_t py, uint16_t spriteAddress, uint32_t height)
    {
        /*
            Draws an N-byte sprite from memory starting at address I to position (Vx, Vy).
            Sets VF to 1 if any pixels are unset due to XOR collision, otherwise 0.
            Drawing is clipped at screen boundaries. Only the 'starting' X and Y wrap around.
        */

        assert(mRAM && "Bus must be set before drawing");

        uint8_t isCollision = 0;

        uint16_t xStart = px % DISPLAY_WIDTH;
        uint16_t yStart = py % DISPLAY_HEIGHT;

        for (uint16_t row = 0; row < height; ++row)
        {
            uint8_t byte = mRAM->Read(spriteAddress + row);
            for (uint8_t bit = 0; bit < SPRITE_ROW_WIDTH; ++bit)
            {
                bool pixelSet = (byte & (0x80 >> bit)) != 0;
                if (!pixelSet)
                {
                    continue;
                }

                uint16_t x = xStart + bit;
                uint16_t y = yStart + row;

                if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
                {
                    continue; // Skip out-of-bounds pixels
                }

                bool wasSet = IsPixelSet(x, y);
                if (wasSet)
                {
                    isCollision = 1;
                }

                SetPixel(x, y, !wasSet); // XOR toggle
            }
        }

        return isCollision;
    }

    void Clear()
    {
        std::fill(mBuffer.begin(), mBuffer.end(), 0);
    }

    bool IsPixelSet(uint32_t px, uint32_t py) const
    {
        assert(px < DISPLAY_WIDTH && py < DISPLAY_HEIGHT);
        return mBuffer[PixelToIndex(px, py)] != 0;
    }

private:
    void SetPixel(uint32_t px, uint32_t py, bool value)
    {
        assert(px < DISPLAY_WIDTH && py < DISPLAY_HEIGHT);
        mBuffer[PixelToIndex(px, py)] = value ? 1 : 0;
    }

    size_t PixelToIndex(uint32_t px, uint32_t py) const
    {
        return px + py * DISPLAY_WIDTH;
    }

    RAM* mRAM = nullptr;
    std::array<uint32_t, DISPLAY_PIXEL_COUNT> mBuffer;
};