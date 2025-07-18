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

// Interpreter state remains unchanged if instruction fails to execute.
//--------------------------------------------------------------------------------
StepResult Interpreter::Step()
{
	const bool kHaltOnFailure = true;

	// Fetch
	const FetchResult fetch = mCPU.Fetch();
	if (!fetch.mIsValidAddress)
	{
		return { fetch.mStatus, kHaltOnFailure };
	}

	// Decode
	const Instruction instruction = mCPU.Decode(fetch.mOpcode);
	if (!instruction.IsValid())
	{
		return { ExecutionStatus::DecodeError, kHaltOnFailure };
	}

	// Execute
	const ExecutionStatus status = mCPU.Execute(instruction);
	switch (status)
	{
		// Waiting for input (no halt, no cycle count)
		case ExecutionStatus::WaitingOnKeyPress:
			return { status, !kHaltOnFailure };
		
		// Instruction executed (advance cycle, no halt)
		case ExecutionStatus::Executed:
			mCycleCount++;
			return { status, !kHaltOnFailure };

		// Error or halt condition (halt, no cycle count)
		default:
			return { status, kHaltOnFailure };
	}
}

//--------------------------------------------------------------------------------
void Interpreter::DecrementTimers()
{
	mCPU.DecrementTimers();
}