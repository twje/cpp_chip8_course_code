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
Interpreter::Interpreter(IRandomProvider& randomProvider)
	: mCPU(mBus, randomProvider)
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

//--------------------------------------------------------------------------------
StepResult Interpreter::Step()
{
	/*
		Performs one fetch-decode-execute step.

		CPU state is only modified if execution succeeds. If decoding fails, execution is 
		deferred (e.g. waiting for a key press), or execution fails, the program counter 
		is rolled back to retry the same instruction.
	*/

	constexpr bool kHaltOnFailure = true;
	const uint16_t pcBeforeFetch = mCPU.GetProgramCounter();

	// Fetch (increments PC)
	const FetchResult fetch = mCPU.Fetch();
	if (!fetch.mIsValidAddress)
	{
		return { fetch.mStatus, kHaltOnFailure };
	}

	// Decode
	const Instruction instruction = mCPU.Decode(fetch.mOpcode);
	if (!instruction.IsValid())
	{
		// Decode failed, so we roll back to preserve CPU state.
		mCPU.SetProgramCounter(pcBeforeFetch);
		return { ExecutionStatus::DECODE_ERROR, kHaltOnFailure };
	}

	// Execute
	const ExecutionStatus status = mCPU.Execute(instruction);
		
	if (status != ExecutionStatus::EXECUTED)
	{
		// Instruction failed or deferred — rollback PC to preserve CPU state
		mCPU.SetProgramCounter(pcBeforeFetch);
	}

	// Handle result
	switch (status)
	{		
		case ExecutionStatus::WaitingOnKeyPress:
			mCycleCount++;
			return { status, !kHaltOnFailure };			
		
		case ExecutionStatus::EXECUTED:
			mCycleCount++;
			return { status, !kHaltOnFailure };	
		
		default:
			return { status, kHaltOnFailure };
	}
}

//--------------------------------------------------------------------------------
void Interpreter::DecrementTimers()
{
	mCPU.DecrementTimers();
}