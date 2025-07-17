#include "Interpreter/Interpreter.h"

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Constants.h"
#include "Interpreter/Instruction/OpcodeTable.h"
#include "Interpreter/Snapshot/SnapshotBuilder.h"

// System
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

//--------------------------------------------------------------------------------
Interpreter::Interpreter()	
	: mCPU(mBus)
	, mCycleCount(0)
{
	mBus.mRAM.WriteRange(0x000, CHAR_SET); // Load CHIP-8 character fontset into memory
	mBus.mDisplay.SetRAM(mBus.mRAM);
}

//--------------------------------------------------------------------------------
void Interpreter::Reset()
{
	mCPU.Reset();
	mBus.mDisplay.Clear();
	mCycleCount = 0;	
}

//--------------------------------------------------------------------------------
bool Interpreter::LoadRom(const std::vector<uint8_t>& data)
{
	// Clear program memory only (preserve fontset in lower RAM)
	mBus.mRAM.ClearProgramMemory();

	if (!mBus.mRAM.WriteRange(PROGRAM_START_ADDRESS, data))
	{
		std::cerr << "ROM too large to fit into memory." << std::endl;
		return false;
	}
	
	return true;
}

//--------------------------------------------------------------------------------
Snapshot Interpreter::PeekNextInstruction() const 
{ 
	SnapshotBuilder builder(mCPU, mCycleCount);
	return builder.Build();
}

// Interpreter state remains unchanged if instruction fails to execute.
//--------------------------------------------------------------------------------
ExecutionStatus Interpreter::Step()
{
	FetchResult fetch = mCPU.FetchOpcode();

	// Attempt to decode the fetched opcode
	Instruction instruction = mCPU.Decode(fetch.mOpcode);
	
	if (!instruction.IsValid())
	{
		mCPU.UndoFetch(fetch.mPreviousPC);
		return ExecutionStatus::DecodeError;
	}

	// Attempt to execute the decoded instruction
	ExecutionStatus status = mCPU.Execute(instruction); // No side effects if execution fails

	if (status != ExecutionStatus::Executed)
	{
		mCPU.UndoFetch(fetch.mPreviousPC);
		return status;
	}

	mCycleCount++;
	return status;
}

//--------------------------------------------------------------------------------
void Interpreter::DecrementTimers()
{

}