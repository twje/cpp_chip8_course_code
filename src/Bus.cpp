#include "Bus.h"

// Includes
#include "Bus.h"

// Chip 8
#include "RAM.h"
#include "Display.h"
#include "Keypad.h"
#include "DelayTimer.h"
#include "SoundTimer.h"

//--------------------------------------------------------------------------------
Bus::Bus(const Subsystems& systems)
    : mRAM(systems.mRAM)
    , mDisplay(systems.mDisplay)
    , mKeypad(systems.mKeypad)
    , mDelayTimer(systems.mDelayTimer)
    , mSoundTimer(systems.mSoundTimer)
{ }

//--------------------------------------------------------------------------------
uint8_t Bus::Read8(uint16_t address) const
{
    return mRAM.Read(address);
}

//--------------------------------------------------------------------------------
void Bus::Write8(uint16_t address, uint8_t value)
{
    mRAM.Write(address, value);
}