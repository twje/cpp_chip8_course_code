#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Config.h"
#include "Bus.h"
#include "EmulationStatus.h"
#include "Instruction.h"

// System
#include <array>

// Macros
//--------------------------------------------------------------------------------
#define DECLARE_OPCODE_HANDLER(name) ExecutionStatus Execute_##name(const Instruction& instruction);

//--------------------------------------------------------------------------------
class CPU
{
#ifdef UNIT_TESTING
	friend class OpcodeTest;
#endif

public:
	CPU(Bus& bus);
	
	void Reset();
	uint8_t GetRegisterValueAt(size_t index) const;
	int16_t GetIndexRegister() const { return mIndexRegister; }
	size_t GetStackPointer() const { return mStackPointer; }
	uint16_t GetStackValueAt(size_t index) const;
	uint8_t GetDelayTimer() const { return mDelayTimer; }
	uint8_t GetSoundTimer() const { return mSoundTimer; }
	
	Instruction Peek();
	Instruction Fetch();
	bool Decode(Instruction& outInstruction);	
	ExecutionStatus Execute(const Instruction& instruction);		

private:	
	bool IsLegacyInstruction(const Instruction& instruction) const;

	// One method per opcode
	DECLARE_OPCODE_HANDLER(CLS)
	DECLARE_OPCODE_HANDLER(RET)
	DECLARE_OPCODE_HANDLER(JP_ADDR)
	DECLARE_OPCODE_HANDLER(CALL_ADDR)
	DECLARE_OPCODE_HANDLER(SE_VX_KK)
	DECLARE_OPCODE_HANDLER(SNE_VX_KK)
	DECLARE_OPCODE_HANDLER(SE_VX_VY)
	DECLARE_OPCODE_HANDLER(LD_VX_KK)
	DECLARE_OPCODE_HANDLER(ADD_VX_KK)
	DECLARE_OPCODE_HANDLER(LD_VX_VY)
	DECLARE_OPCODE_HANDLER(OR_VX_VY)
	DECLARE_OPCODE_HANDLER(AND_VX_VY)
	DECLARE_OPCODE_HANDLER(XOR_VX_VY)
	DECLARE_OPCODE_HANDLER(ADD_VX_VY)
	DECLARE_OPCODE_HANDLER(SUB_VX_VY)
	DECLARE_OPCODE_HANDLER(SHR_VX_VY)
	DECLARE_OPCODE_HANDLER(SUBN_VX_VY)
	DECLARE_OPCODE_HANDLER(SHL_VX_VY)
	DECLARE_OPCODE_HANDLER(SNE_VX_VY)
	DECLARE_OPCODE_HANDLER(LD_I_ADDR)
	DECLARE_OPCODE_HANDLER(JP_V0_ADDR)
	DECLARE_OPCODE_HANDLER(RND_VX_KK)
	DECLARE_OPCODE_HANDLER(DRW_VX_VY_N)
	DECLARE_OPCODE_HANDLER(SKP_VX)
	DECLARE_OPCODE_HANDLER(SKNP_VX)
	DECLARE_OPCODE_HANDLER(LD_VX_DT)
	DECLARE_OPCODE_HANDLER(LD_VX_K)
	DECLARE_OPCODE_HANDLER(LD_DT_VX)
	DECLARE_OPCODE_HANDLER(LD_ST_VX)
	DECLARE_OPCODE_HANDLER(ADD_I_VX)
	DECLARE_OPCODE_HANDLER(LD_F_VX)
	DECLARE_OPCODE_HANDLER(LD_B_VX)
	DECLARE_OPCODE_HANDLER(LD_I_VX)
	DECLARE_OPCODE_HANDLER(LD_VX_I)

	Bus& mBus;

	// Core CPU State
	uint16_t mProgramCounter;
	uint16_t mIndexRegister;
	std::array<uint8_t, NUM_REGISTERS> mRegisters; // V0–VF

	// Stack
	size_t mStackPointer;
	std::array<uint16_t, STACK_SIZE> mStack;

	// Timers
	uint8_t mDelayTimer;
	uint8_t mSoundTimer;
};