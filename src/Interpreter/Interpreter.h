#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Constants.h"
#include "Interpreter/Bus.h"
#include "Interpreter/Hardware/CPU.h"
#include "Types/ExecutionStatus.h"
#include "Interpreter/Instruction/Instruction.h"
#include "Interpreter/Snapshot/Snapshot.h"

// System
#include <vector>
#include <memory>

//--------------------------------------------------------------------------------
class Interpreter
{
#ifdef UNIT_TESTING
	friend class OpcodeTest;
#endif

public:
	Interpreter();	

	void Reset();
	bool LoadRom(const std::vector<uint8_t>& data);

	Snapshot PeekNextInstruction() const;	
	ExecutionStatus Step();
	void DecrementTimers();

	const CPU& GetCPU() const { return mCPU; }
	const Bus& GetBus() const { return mBus; }
	Bus& GetBus() { return mBus; }

private:
	Bus mBus;
	CPU mCPU;
	size_t mCycleCount;
};