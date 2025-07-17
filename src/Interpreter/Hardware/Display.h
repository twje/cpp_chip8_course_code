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
public:
    Display()
        : mBuffer{ }
    { }

    void SetRAM(RAM& ram) { mRAM = &ram; }

    // Draws a CHIP-8 sprite from memory and returns 1 if any pixels were erased (for VF register)
    uint8_t DrawSprite(uint32_t px, uint32_t py, uint16_t spriteAddress, uint32_t height)
    {
        assert(mRAM && "Bus must be set before drawing");

        uint8_t isCollision = 0;

        for (uint16_t row = 0; row < height; ++row)
        {
            uint8_t byte = mRAM->Read(spriteAddress + row);
            for (uint8_t bit = 0; bit < 8; ++bit)
            {
                bool pixelSet = (byte & (0x80 >> bit)) != 0;
                if (!pixelSet)
                {
                    continue;
                }

                uint16_t x = (px + bit) % DISPLAY_WIDTH;
                uint16_t y = (py + row) % DISPLAY_HEIGHT;

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

    bool IsPixelSet(uint32_t px, uint32_t py) const
    {
        assert(px < DISPLAY_WIDTH && py < DISPLAY_HEIGHT);
        return mBuffer[PixelToIndex(px, py)] != 0;
    }

    void Clear()
    {
        std::fill(mBuffer.begin(), mBuffer.end(), 0);
    }

    void SetPixel(uint32_t px, uint32_t py, bool value)
    {
        assert(px < DISPLAY_WIDTH && py < DISPLAY_HEIGHT);
        mBuffer[PixelToIndex(px, py)] = value ? 1 : 0;
    }

private:
    size_t PixelToIndex(uint32_t px, uint32_t py) const
    {
        return px + py * DISPLAY_WIDTH;
    }

private:
    RAM* mRAM = nullptr;
    std::array<uint32_t, DISPLAY_PIXEL_COUNT> mBuffer;    
};