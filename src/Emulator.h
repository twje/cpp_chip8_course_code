#pragma once

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "Common.h"
#include "RAM.h"
#include "Display.h"
#include "Keypad.h"
#include "DelayTimer.h"
#include "SoundTimer.h"
#include "Bus.h"
#include "CPU.h"

// System
#include <array>

//--------------------------------------------------------------------------------
class Emulator
{
#ifdef UNIT_TESTING
	friend class CPUOpcodeTest;
#endif

public:
	Emulator();

	bool LoadRom(const fs::path& romPath);
	void Step();

private:
	RAM mRAM;
	Display mDisplay;
	Keypad mKeypad;
	DelayTimer mDelayTimer;
	SoundTimer mSoundTimer;

	Bus mBus;
	CPU mCPU;
};