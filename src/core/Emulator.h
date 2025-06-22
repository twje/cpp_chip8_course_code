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

// TODO: think about removing 'Bus& GetBus() { return mBus; }' and adding unit test define so bus can be maniupated
//--------------------------------------------------------------------------------
class Emulator
{
#ifdef UNIT_TESTING
	friend class OpcodeTest;
#endif

public:
	Emulator();

	void Reset();
	bool LoadRom(const fs::path& romPath);
	
	PeekResult PeekNextInstruction();
	StepResult Step();
	
	size_t GetCycle() const { return mCycle; }
	const CPU& GetCPU() const { return mCPU; }
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