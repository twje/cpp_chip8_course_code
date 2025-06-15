#pragma once

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "Config.h"

// System
#include <cstdint>
#include <array>

//--------------------------------------------------------------------------------
class RAM;
class Display;
class Keypad;
class DelayTimer;
class SoundTimer;

//--------------------------------------------------------------------------------
struct Subsystems
{
    RAM& mRAM;
    Display& mDisplay;
    Keypad& mKeypad;
    DelayTimer& mDelayTimer;
    SoundTimer& mSoundTimer;
};

//--------------------------------------------------------------------------------
class Bus
{
public:
    Bus(const Subsystems& systems);

    uint8_t Read8(uint16_t address) const;
    void Write8(uint16_t address, uint8_t value);

private:
    RAM& mRAM;
    Display& mDisplay;
    Keypad& mKeypad;
    DelayTimer& mDelayTimer;
    SoundTimer& mSoundTimer;
};