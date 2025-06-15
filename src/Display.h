#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Config.h"
#include "Bus.h"

// System
#include <array>
#include <cassert>

//--------------------------------------------------------------------------------
class Display
{
public:
    void SetBus(Bus& bus) { mBus = &bus; }

    // Draws a CHIP-8 sprite from memory and returns 1 if any pixels were erased (for VF register)
    uint8_t DrawSprite(uint32_t px, uint32_t py, size_t spriteAddress, uint32_t height)
    {
        assert(mBus && "Bus must be set before drawing");

        uint8_t collision = 0;

        for (uint32_t row = 0; row < height; ++row)
        {
            uint8_t byte = mBus->Read8(spriteAddress + row);
            for (uint8_t bit = 0; bit < 8; ++bit)
            {
                // Skip if the current bit (pixel) is not set
                if ((byte & (0x80 >> bit)) == 0)
                {
                    continue;
                }

                uint32_t x = (px + bit) % DISPLAY_WIDTH;
                uint32_t y = (py + row) % DISPLAY_HEIGHT;

                bool current = IsPixelSet(x, y);
                if (current)
                {
                    collision = 1;
                }

                SetPixel(x, y, !current); // XOR toggle
            }
        }

        return collision;
    }
    
    bool GetPixel(size_t index) const
    {
        assert(index < mBuffer.size());
        return mBuffer[index] != 0;
    }

    size_t GetPixelCount() const
    {
        return mBuffer.size();
    }

private:
    void SetPixel(uint32_t px, uint32_t py, bool value)
    {
        assert(px < DISPLAY_WIDTH && py < DISPLAY_HEIGHT);
        mBuffer[px + py * DISPLAY_WIDTH] = value ? 1 : 0;
    }

    bool IsPixelSet(uint32_t px, uint32_t py) const
    {
        assert(px < DISPLAY_WIDTH && py < DISPLAY_HEIGHT);
        return mBuffer[px + py * DISPLAY_WIDTH] != 0;
    }

private:
    Bus* mBus = nullptr;
    std::array<uint32_t, DISPLAY_PIXEL_COUNT> mBuffer;
};
