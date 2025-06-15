#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Common.h"
#include "Config.h"

// System
#include <array>
#include <cstdint>
#include <span>

//--------------------------------------------------------------------------------
class RAM 
{
public:
    uint8_t Read(uint16_t address) const;
    void Write(uint16_t address, uint8_t value);
    bool WriteRange(size_t start, std::span<const uint8_t> data);
    void ClearProgramMemory();

private:
    std::array<uint8_t, RAM_SIZE> mData;
};
