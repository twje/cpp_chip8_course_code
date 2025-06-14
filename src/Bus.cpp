#include "Bus.h"

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "Display.h"
#include "Keypad.h"
#include "DelayTimer.h"
#include "SoundTimer.h"

//--------------------------------------------------------------------------------
Bus::Bus(const BusConfig& config)
    : mRam(config.mRam)
    , mDisplay(config.mDisplay)
    , mKeypad(config.mKeypad)
    , mDelayTimer(config.mDelayTimer)
    , mSoundTimer(config.mSoundTimer)
{ }

//--------------------------------------------------------------------------------
uint8_t Bus::Read8(uint16_t address) const
{
    return 0;
}

//--------------------------------------------------------------------------------
void Bus::Write8(uint16_t address, uint8_t value)
{

}