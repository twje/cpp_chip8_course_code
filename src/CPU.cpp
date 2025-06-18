#include "CPU.h"

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Config.h"
#include "Instruction.h"
#include "RAM.h"
#include "Display.h"
#include "Keypad.h"
#include "DelayTimer.h"
#include "SoundTimer.h"

// System
#include <optional>
#include <iostream>
#include <cassert>

//--------------------------------------------------------------------------------
CPU::CPU(Bus& bus)
	: mBus{ bus }
    , mPC(PROGRAM_START_ADDRESS)
{ }

// CHIP-8 stores opcodes as two consecutive bytes in big-endian format.
// Read and combine the two bytes into a single 16-bit opcode.
//--------------------------------------------------------------------------------
Instruction CPU::Fetch()
{
    const uint16_t address = mPC;
    assert(address % 2 == 0);

    const uint8_t hByte = mBus.mRAM.Read(address);
    const uint8_t lByte = mBus.mRAM.Read(address + 1);
    
    const uint16_t opcode = (static_cast<uint16_t>(hByte) << 8) | lByte;
    Instruction instruction(address, opcode);

    // Advance PC early (some instructions override it)
    mPC += 2;

    return instruction;
}

//--------------------------------------------------------------------------------
bool CPU::Decode(Instruction& outInstruction)
{
    for (const OpcodePatternDef& opcodeDef : OPCODE_PATTERN_TABLE)
    {
        if (outInstruction.MatchesPattern(opcodeDef))
        {
            outInstruction.DecodeFrom(opcodeDef);
            return true;
        }
    }
    
    return false;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute(const Instruction& instruction)
{
    if (IsLegacyInstruction(instruction))
    {
        return ExecutionStatus::Ignored;
    }

    switch (instruction.GetPatternId())
    {
        case OpcodePatternId::CLS:           return Execute_CLS(instruction);
        case OpcodePatternId::RET:           return Execute_RET(instruction);
        case OpcodePatternId::JP_ADDR:       return Execute_JP_ADDR(instruction);
        case OpcodePatternId::CALL_ADDR:     return Execute_CALL_ADDR(instruction);
        case OpcodePatternId::SE_VX_KK:      return Execute_SE_VX_KK(instruction);
        case OpcodePatternId::SNE_VX_KK:     return Execute_SNE_VX_KK(instruction);
        case OpcodePatternId::SE_VX_VY:      return Execute_SE_VX_VY(instruction);
        case OpcodePatternId::LD_VX_KK:      return Execute_LD_VX_KK(instruction);
        case OpcodePatternId::ADD_VX_KK:     return Execute_ADD_VX_KK(instruction);
        case OpcodePatternId::LD_VX_VY:      return Execute_LD_VX_VY(instruction);
        case OpcodePatternId::OR_VX_VY:      return Execute_OR_VX_VY(instruction);
        case OpcodePatternId::AND_VX_VY:     return Execute_AND_VX_VY(instruction);
        case OpcodePatternId::XOR_VX_VY:     return Execute_XOR_VX_VY(instruction);
        case OpcodePatternId::ADD_VX_VY:     return Execute_ADD_VX_VY(instruction);
        case OpcodePatternId::SUB_VX_VY:     return Execute_SUB_VX_VY(instruction);
        case OpcodePatternId::SHR_VX_VY:     return Execute_SHR_VX_VY(instruction);
        case OpcodePatternId::SUBN_VX_VY:    return Execute_SUBN_VX_VY(instruction);
        case OpcodePatternId::SHL_VX_VY:     return Execute_SHL_VX_VY(instruction);
        case OpcodePatternId::SNE_VX_VY:     return Execute_SNE_VX_VY(instruction);
        case OpcodePatternId::LD_I_ADDR:     return Execute_LD_I_ADDR(instruction);
        case OpcodePatternId::JP_V0_ADDR:    return Execute_JP_V0_ADDR(instruction);
        case OpcodePatternId::RND_VX_KK:     return Execute_RND_VX_KK(instruction);
        case OpcodePatternId::DRW_VX_VY_N:   return Execute_DRW_VX_VY_N(instruction);
        case OpcodePatternId::SKP_VX:        return Execute_SKP_VX(instruction);
        case OpcodePatternId::SKNP_VX:       return Execute_SKNP_VX(instruction);
        case OpcodePatternId::LD_VX_DT:      return Execute_LD_VX_DT(instruction);
        case OpcodePatternId::LD_VX_K:       return Execute_LD_VX_K(instruction);
        case OpcodePatternId::LD_DT_VX:      return Execute_LD_DT_VX(instruction);
        case OpcodePatternId::LD_ST_VX:      return Execute_LD_ST_VX(instruction);
        case OpcodePatternId::ADD_I_VX:      return Execute_ADD_I_VX(instruction);
        case OpcodePatternId::LD_F_VX:       return Execute_LD_F_VX(instruction);
        case OpcodePatternId::LD_B_VX:       return Execute_LD_B_VX(instruction);
        case OpcodePatternId::LD_I_VX:       return Execute_LD_I_VX(instruction);
        case OpcodePatternId::LD_VX_I:       return Execute_LD_VX_I(instruction);
    }
        
    return ExecutionStatus::MissingHandler;
}

//--------------------------------------------------------------------------------
bool CPU::IsLegacyInstruction(const Instruction& instruction) const
{
    // Ignore 0nnn (RCA 1802 call) — obsolete on modern interpreters
    if (instruction.GetPatternId() == OpcodePatternId::SYS_ADDR)
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_CLS(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_CLS" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_RET(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_RET" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_JP_ADDR(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_JP_ADDR" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_CALL_ADDR(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_CALL_ADDR" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SE_VX_KK(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SE_VX_KK" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SNE_VX_KK(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SNE_VX_KK" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SE_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SE_VX_VY" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_VX_KK(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_VX_KK" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_ADD_VX_KK(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_ADD_VX_KK" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_VX_VY" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_OR_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_OR_VX_VY" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_AND_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_AND_VX_VY" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_XOR_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_XOR_VX_VY" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_ADD_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_ADD_VX_VY" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SUB_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SUB_VX_VY" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SHR_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SHR_VX_VY" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SUBN_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SUBN_VX_VY" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SHL_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SHL_VX_VY" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SNE_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SNE_VX_VY" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_I_ADDR(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_I_ADDR" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_JP_V0_ADDR(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_JP_V0_ADDR" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_RND_VX_KK(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_RND_VX_KK" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_DRW_VX_VY_N(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_DRW_VX_VY_N" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SKP_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SKP_VX" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SKNP_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SKNP_VX" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_VX_DT(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_VX_DT" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_VX_K(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_VX_K" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_DT_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_DT_VX" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_ST_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_ST_VX" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_ADD_I_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_ADD_I_VX" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_F_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_F_VX" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_B_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_B_VX" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_I_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_I_VX" << std::endl;
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_VX_I(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_VX_I" << std::endl;
    return ExecutionStatus::NotImplemented;
}