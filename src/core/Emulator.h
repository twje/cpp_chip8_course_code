#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Bus.h"
#include "CPU.h"
#include "Common.h"
#include "DelayTimer.h"
#include "Display.h"
#include "ExecutionStatus.h"
#include "Instruction.h"
#include "Keypad.h"
#include "RAM.h"
#include "SoundTimer.h"

// Third Party

// System
#include <vector>

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
	ExecutionStatus Step();

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