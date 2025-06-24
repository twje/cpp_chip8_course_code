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
#include <optional>

// Macros
//--------------------------------------------------------------------------------
#define DECLARE_OPCODE_HANDLER(pattern, mnemonic) ExecutionStatus Execute_##pattern##_##mnemonic(const Instruction& instruction);

//--------------------------------------------------------------------------------
struct DecodeResult
{
	DecodeStatus status = DecodeStatus::UNKNOWN_OPCODE;
	std::optional<Instruction> mInstruction;
};

//--------------------------------------------------------------------------------
struct AddressOpcode
{
	uint16_t mAddress = 0;
	uint16_t mOpcode = 0;
};

//--------------------------------------------------------------------------------
struct CPUState
{
	// Core CPU State
	std::array<uint8_t, REGISTER_COUNT> mV{ };
	uint16_t mI = 0;
	uint16_t mPC = 0;
	uint16_t mCycle = 0;
	
	// Stack
	uint8_t mSP = 0;
	std::array<uint16_t, STACK_SIZE> mStack{ };
	
	// Timers
	uint8_t mDelayTimer = 0;
	uint8_t mSoundTimer = 0;
};

//--------------------------------------------------------------------------------
class CPU
{
#ifdef UNIT_TESTING
	friend class OpcodeTest;
#endif

public:
	CPU(Bus& bus);

	void Reset();
	const CPUState& GetState() const { return mState; }

	AddressOpcode Peek() const;
	AddressOpcode Fetch();
	DecodeResult Decode(uint16_t opcode) const;
	ExecutionStatus Execute(const Instruction& instruction);	

private:
	// One method per opcode
	DECLARE_OPCODE_HANDLER(0nnn, SYS_ADDR)
	DECLARE_OPCODE_HANDLER(00E0, CLS)
	DECLARE_OPCODE_HANDLER(00EE, RET)
	DECLARE_OPCODE_HANDLER(1nnn, JP_ADDR)
	DECLARE_OPCODE_HANDLER(2nnn, CALL_ADDR)
	DECLARE_OPCODE_HANDLER(3xkk, SE_VX_KK)
	DECLARE_OPCODE_HANDLER(4xkk, SNE_VX_KK)
	DECLARE_OPCODE_HANDLER(5xy0, SE_VX_VY)
	DECLARE_OPCODE_HANDLER(6xkk, LD_VX_KK)
	DECLARE_OPCODE_HANDLER(7xkk, ADD_VX_KK)
	DECLARE_OPCODE_HANDLER(8xy0, LD_VX_VY)
	DECLARE_OPCODE_HANDLER(8xy1, OR_VX_VY)
	DECLARE_OPCODE_HANDLER(8xy2, AND_VX_VY)
	DECLARE_OPCODE_HANDLER(8xy3, XOR_VX_VY)
	DECLARE_OPCODE_HANDLER(8xy4, ADD_VX_VY)
	DECLARE_OPCODE_HANDLER(8xy5, SUB_VX_VY)
	DECLARE_OPCODE_HANDLER(8xy6, SHR_VX_VY)
	DECLARE_OPCODE_HANDLER(8xy7, SUBN_VX_VY)
	DECLARE_OPCODE_HANDLER(8xyE, SHL_VX_VY)
	DECLARE_OPCODE_HANDLER(9xy0, SNE_VX_VY)
	DECLARE_OPCODE_HANDLER(Annn, LD_I_ADDR)
	DECLARE_OPCODE_HANDLER(Bnnn, JP_V0_ADDR)
	DECLARE_OPCODE_HANDLER(Cxkk, RND_VX_KK)
	DECLARE_OPCODE_HANDLER(Dxyn, DRW_VX_VY_N)
	DECLARE_OPCODE_HANDLER(Ex9E, SKP_VX)
	DECLARE_OPCODE_HANDLER(ExA1, SKNP_VX)
	DECLARE_OPCODE_HANDLER(Fx07, LD_VX_DT)
	DECLARE_OPCODE_HANDLER(Fx0A, LD_VX_K)
	DECLARE_OPCODE_HANDLER(Fx15, LD_DT_VX)
	DECLARE_OPCODE_HANDLER(Fx18, LD_ST_VX)
	DECLARE_OPCODE_HANDLER(Fx1E, ADD_I_VX)
	DECLARE_OPCODE_HANDLER(Fx29, LD_F_VX)
	DECLARE_OPCODE_HANDLER(Fx33, LD_B_VX)
	DECLARE_OPCODE_HANDLER(Fx55, LD_I_VX)
	DECLARE_OPCODE_HANDLER(Fx65, LD_VX_I)

	Bus& mBus;
	CPUState mState;
};