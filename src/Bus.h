#pragma once

// Forward Declarations
//--------------------------------------------------------------------------------
class RAM;
class Display;
class Keypad;
class DelayTimer;
class SoundTimer;

//--------------------------------------------------------------------------------
struct Bus
{
    RAM& mRAM;
    Display& mDisplay;
    Keypad& mKeypad;
    DelayTimer& mDelayTimer;
    SoundTimer& mSoundTimer;
};