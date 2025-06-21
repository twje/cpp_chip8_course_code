#pragma once

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "EmulationStatus.h"
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
public:
	Emulator();

	void Reset();
	bool LoadRom(const fs::path& romPath);
	
	PeekResult PeekNextInstruction();
	StepResult Step();
		
	CPU& GetCPU() { return mCPU; }
	const CPU& GetCPU() const { return mCPU; }
	
	Bus& GetBus() { return mBus; }
	const Bus& GetBus() const { return mBus; }

private:
	RAM mRAM;
	Display mDisplay;
	Keypad mKeypad;
	DelayTimer mDelayTimer;
	SoundTimer mSoundTimer;
	size_t mCycle;

	Bus mBus;
	CPU mCPU;	
};