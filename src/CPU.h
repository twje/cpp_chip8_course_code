#include <iostream>

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Bus.h"
#include "Disassembler.h"
#include "StepResult.h"

// System
#include <array>

// Macros
//--------------------------------------------------------------------------------
#define DECLARE_OPCODE_HANDLER(name) bool Execute_##name(const Instruction& instruction);

//--------------------------------------------------------------------------------
class CPU
{
#ifdef UNIT_TESTING
	friend class CPUOpcodeTest;
#endif

public:
	CPU(Bus& bus);

	StepResult Step();

private:
	uint16_t Fetch(uint16_t address);
	bool Execute(const Instruction& instruction);	
	
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
	Disassembler mDisassembler;	
	uint16_t mPC;
};