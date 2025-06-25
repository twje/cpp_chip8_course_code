#include "Emulator.h"

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Config.h"

// System
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

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
InstructionInfo Emulator::PreviewInstruction() const
{
	const uint16_t address = mCPU.GetState().mPC;
	const uint16_t opcode = mCPU.PeekNextOpcode();

	Instruction instruction = mCPU.Decode(opcode);

	InstructionInfo info;
	info.mAddress = address;
	info.mOpcode = opcode;
	info.mDecodeSucceeded = (instruction.mOpcodeId != OpcodeId::UNASSIGNED);

	if (!info.mDecodeSucceeded)
	{
		return info;
	}

	const OpcodeSpec& spec = OPCODE_TABLE.at(instruction.mOpcodeId);
	info.mPattern = spec.mPatternStr;
	info.mMnemonic = spec.mMnemonic;
	info.mOperands = FormatOperands(spec.mOperands, instruction.mOperands);
	info.mPreviewCycle = mCycle;

	return info;
}

//--------------------------------------------------------------------------------
ExecutionStatus Emulator::Step()
{	
	uint16_t opcode = mCPU.FetchOpcode();
	Instruction instruction = mCPU.Decode(opcode);

	if (instruction.mOpcodeId == OpcodeId::UNASSIGNED)
	{
		return ExecutionStatus::DecodeError;
	}

	ExecutionStatus status = mCPU.Execute(instruction);
	mCycle++;

	return status;
}

//--------------------------------------------------------------------------------
std::vector<OperandInfo> Emulator::FormatOperands(const std::vector<OperandSpec>& operandSpecs, const std::vector<uint16_t>& values) const
{
	assert(operandSpecs.size() == values.size() && "Operand defs and values size mismatch");

	std::vector<OperandInfo> formatted;
	formatted.reserve(operandSpecs.size());

	for (size_t i = 0; i < operandSpecs.size(); ++i)
	{
		formatted.push_back({ operandSpecs[i].mLabel, values[i] });
	}

	return formatted;
}