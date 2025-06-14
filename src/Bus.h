#pragma once

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "Config.h"

// System
#include <cstdint>
#include <array>

//--------------------------------------------------------------------------------
class Display;
class Keypad;
class DelayTimer;
class SoundTimer;

//--------------------------------------------------------------------------------
struct BusConfig 
{
    std::array<uint8_t, RAM_SIZE>& mRam;
    Display& mDisplay;
    Keypad& mKeypad;
    DelayTimer& mDelayTimer;
    SoundTimer& mSoundTimer;
};

//--------------------------------------------------------------------------------
class Bus
{
public:
    Bus(const BusConfig& config);

    uint8_t Read8(uint16_t address) const;
    void Write8(uint16_t address, uint8_t value);

private:
    std::array<uint8_t, RAM_SIZE>& mRam;
    Display& mDisplay;
    Keypad& mKeypad;
    DelayTimer& mDelayTimer;
    SoundTimer& mSoundTimer;
};