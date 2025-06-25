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
	friend class OpcodeTest;
#endif

public:
	Emulator();

	void Reset();
	bool LoadRom(const fs::path& romPath);	

	InstructionInfo PreviewInstruction() const;
	StepResult Step();

	const CPU& GetCPU() const { return mCPU; }
	const Bus& GetBus() const { return mBus; }

private:
	std::vector<OperandInfo> FormatOperands(const std::vector<OperandSpec>& operandSpecs, const std::vector<uint16_t>& values) const;

	RAM mRAM;
	Display mDisplay;
	Keypad mKeypad;
	DelayTimer mDelayTimer;
	SoundTimer mSoundTimer;
	size_t mCycle;

	Bus mBus;
	CPU mCPU;	
};