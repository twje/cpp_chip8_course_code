#pragma once

// Includes
//--------------------------------------------------------------------------------
// Chip 8
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
public:
	Emulator();

	void LoadRom();
	void Run();

private:
	std::array<uint8_t, RAM_SIZE> mRAM;
	Display mDisplay;
	Keypad mKeypad;
	DelayTimer mDelayTimer;
	SoundTimer mSoundTimer;

	Bus mBus;
	CPU mCPU;
};