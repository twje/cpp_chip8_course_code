#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interpreter/Hardware/DelayTimer.h"
#include "Interpreter/Hardware/Display.h"
#include "Interpreter/Hardware/Keypad.h"
#include "Interpreter/Hardware/RAM.h"
#include "Interpreter/Hardware/SoundTimer.h"

//--------------------------------------------------------------------------------
struct Bus
{
    RAM mRAM;
    Display mDisplay;
    Keypad mKeypad;
    DelayTimer mDelayTimer;
    SoundTimer mSoundTimer;    
};