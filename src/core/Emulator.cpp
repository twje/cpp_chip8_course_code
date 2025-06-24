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
InstructionInfo Emulator::PreviewInstruction() const
{
	InstructionInfo info;

	AddressOpcode raw = mCPU.Peek();
	info.mAddress = raw.mAddress;
	info.mOpcode = raw.mOpcode;	

	DecodeResult decode = mCPU.Decode(raw.mOpcode);
	info.mDecodeStatus = decode.status;

	if (decode.status == DecodeStatus::UNKNOWN_OPCODE)
	{
		return info;
	}

	assert(decode.mInstruction);
	const Instruction& instruction = decode.mInstruction.value();

	OpcodeDisplayInfo display = GetOpcodeDisplayInfo(instruction.mOpcodeId);
	info.mPattern = display.mPattern;
	info.mMnemonic = display.mMnemonic;

	const OpcodeFormat& def = OPCODE_FORMAT_MAP.at(instruction.mOpcodeId);
	info.mOperands = FormatOperands(def.mOperands, instruction.mOperands);

	info.mPreviewCycle = mCycle;

	return info;
}

//--------------------------------------------------------------------------------
StepResult Emulator::Step()
{	
	AddressOpcode raw = mCPU.Fetch();	
	DecodeResult decode = mCPU.Decode(raw.mOpcode);

	if (decode.status == DecodeStatus::UNKNOWN_OPCODE)
	{
		return { decode.mInstruction, ExecutionStatus::DecodeError };
	}

	assert(decode.mInstruction);
	const Instruction& instruction = decode.mInstruction.value();
	
	ExecutionStatus status = mCPU.Execute(instruction);
	mCycle++;

	return { decode.mInstruction, status };
}

//--------------------------------------------------------------------------------
std::vector<OperandInfo> Emulator::FormatOperands(const std::vector<OperandFormat>& defs, const std::vector<uint16_t>& values) const
{
	assert(defs.size() == values.size() && "Operand defs and values size mismatch");

	std::vector<OperandInfo> formatted;
	formatted.reserve(defs.size());

	for (size_t i = 0; i < defs.size(); ++i)
	{
		formatted.push_back({ OperandLabelToString(defs[i].mLabel), values[i] });
	}

	return formatted;
}