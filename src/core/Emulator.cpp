#include "Emulator.h"

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Config.h"

// System
#include <fstream>
#include <iostream>

//--------------------------------------------------------------------------------
Emulator::Emulator()
	: mBus({ mRAM, mDisplay, mKeypad, mDelayTimer, mSoundTimer })
	, mCPU(mBus)
	, mCycle(0)
{
	mRAM.WriteRange(0x000, CHAR_SET); // Load CHIP-8 character fontset into memory
	mDisplay.SetRAM(mRAM);
}

//--------------------------------------------------------------------------------
void Emulator::Reset()
{
	mRAM.ClearProgramMemory();	
	mCPU.Reset();
	mCycle = 0;

	// TODO: reste bus components
}

//--------------------------------------------------------------------------------
bool Emulator::LoadRom(const fs::path& romPath)
{
	// Clear program memory only (preserve fontset in lower RAM)
	mRAM.ClearProgramMemory();

	std::ifstream file(romPath, std::ios::binary);
	if (!file)
	{
		std::cerr << "Could not open file: " << romPath << std::endl;
		return false;
	}

	// Determine size
	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (fileSize <= 0)
	{
		std::cerr << "ROM file is empty." << std::endl;
		return false;
	}

	if (fileSize > (RAM_SIZE - PROGRAM_START_ADDRESS))
	{
		std::cerr << "ROM too large to fit in memory." << std::endl;
		return false;
	}

	std::vector<uint8_t> buffer(static_cast<size_t>(fileSize));
	if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
	{
		std::cerr << "Failed to read full ROM into memory." << std::endl;
		return false;
	}

	if (!mRAM.WriteRange(PROGRAM_START_ADDRESS, buffer)) 
	{
		std::cerr << "ROM too large to fit into memory." << std::endl;
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------
PeekResult Emulator::PeekNextInstruction()
{
	Instruction instruction = mCPU.Peek();
	if (!mCPU.Decode(instruction))
	{
		return { instruction, PeekStatus::DecodeError };
	}	

	return { instruction, PeekStatus::Valid };
}

//--------------------------------------------------------------------------------
StepResult Emulator::Step()
{
	Instruction instruction = mCPU.Fetch();	

	if (!mCPU.Decode(instruction))
	{
		return { instruction, ExecutionStatus::DecodeError };
	}
	
	ExecutionStatus result = mCPU.Execute(instruction);
	mCycle++;
	
	return { instruction, result };
}