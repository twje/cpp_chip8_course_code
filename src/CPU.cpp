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

// Fetch, decode and execute an opcode
//--------------------------------------------------------------------------------
StepResult CPU::Step()
{    
    Instruction instruction = Fetch();

    // Advance PC early (some instructions override it)
    mPC += 2;
        
    // Decode instruction
    if (!mDisassembler.Decode(instruction))
    {
        return { StepResultStatus::DecodeError, instruction };
    }

    if (IsLegacyInstruction(instruction))
    {
        return { StepResultStatus::Ignored, instruction };
    }

    bool wasExecuted = Execute(instruction);
    if (wasExecuted)
    {
        return { StepResultStatus::Executed, instruction };
    }

    return { StepResultStatus::Unimplemented, instruction };
}

//--------------------------------------------------------------------------------
Instruction CPU::Fetch()
{
    const uint16_t address = mPC;
    assert(address % 2 == 0);

    const uint8_t hByte = mBus.mRAM.Read(address);
    const uint8_t lByte = mBus.mRAM.Read(address + 1);

    const uint16_t opcode = (static_cast<uint16_t>(hByte) << 8) | lByte;
    return Instruction(opcode, address);
}

//--------------------------------------------------------------------------------
bool CPU::IsLegacyInstruction(Instruction& instruction)
{
    // Ignore 0nnn (RCA 1802 call) — obsolete on modern interpreters
    if (instruction.mInstructionPatternId == InstructionPatternId::SYS_ADDR)
    {
        return true;
    }
    
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute(const Instruction& instruction)
{
    switch (instruction.mInstructionPatternId) // TODO: think about changing to ID?
    {
        case InstructionPatternId::CLS:           return Execute_CLS(instruction);
        case InstructionPatternId::RET:           return Execute_RET(instruction);
        case InstructionPatternId::JP_ADDR:       return Execute_JP_ADDR(instruction);
        case InstructionPatternId::CALL_ADDR:     return Execute_CALL_ADDR(instruction);
        case InstructionPatternId::SE_VX_KK:      return Execute_SE_VX_KK(instruction);
        case InstructionPatternId::SNE_VX_KK:     return Execute_SNE_VX_KK(instruction);
        case InstructionPatternId::SE_VX_VY:      return Execute_SE_VX_VY(instruction);
        case InstructionPatternId::LD_VX_KK:      return Execute_LD_VX_KK(instruction);
        case InstructionPatternId::ADD_VX_KK:     return Execute_ADD_VX_KK(instruction);
        case InstructionPatternId::LD_VX_VY:      return Execute_LD_VX_VY(instruction);
        case InstructionPatternId::OR_VX_VY:      return Execute_OR_VX_VY(instruction);
        case InstructionPatternId::AND_VX_VY:     return Execute_AND_VX_VY(instruction);
        case InstructionPatternId::XOR_VX_VY:     return Execute_XOR_VX_VY(instruction);
        case InstructionPatternId::ADD_VX_VY:     return Execute_ADD_VX_VY(instruction);
        case InstructionPatternId::SUB_VX_VY:     return Execute_SUB_VX_VY(instruction);
        case InstructionPatternId::SHR_VX_VY:     return Execute_SHR_VX_VY(instruction);
        case InstructionPatternId::SUBN_VX_VY:    return Execute_SUBN_VX_VY(instruction);
        case InstructionPatternId::SHL_VX_VY:     return Execute_SHL_VX_VY(instruction);
        case InstructionPatternId::SNE_VX_VY:     return Execute_SNE_VX_VY(instruction);
        case InstructionPatternId::LD_I_ADDR:     return Execute_LD_I_ADDR(instruction);
        case InstructionPatternId::JP_V0_ADDR:    return Execute_JP_V0_ADDR(instruction);
        case InstructionPatternId::RND_VX_KK:     return Execute_RND_VX_KK(instruction);
        case InstructionPatternId::DRW_VX_VY_N:   return Execute_DRW_VX_VY_N(instruction);
        case InstructionPatternId::SKP_VX:        return Execute_SKP_VX(instruction);
        case InstructionPatternId::SKNP_VX:       return Execute_SKNP_VX(instruction);
        case InstructionPatternId::LD_VX_DT:      return Execute_LD_VX_DT(instruction);
        case InstructionPatternId::LD_VX_K:       return Execute_LD_VX_K(instruction);
        case InstructionPatternId::LD_DT_VX:      return Execute_LD_DT_VX(instruction);
        case InstructionPatternId::LD_ST_VX:      return Execute_LD_ST_VX(instruction);
        case InstructionPatternId::ADD_I_VX:      return Execute_ADD_I_VX(instruction);
        case InstructionPatternId::LD_F_VX:       return Execute_LD_F_VX(instruction);
        case InstructionPatternId::LD_B_VX:       return Execute_LD_B_VX(instruction);
        case InstructionPatternId::LD_I_VX:       return Execute_LD_I_VX(instruction);
        case InstructionPatternId::LD_VX_I:       return Execute_LD_VX_I(instruction);
    }
        
    return false; // Unimplemented instruction
}

//--------------------------------------------------------------------------------
bool CPU::Execute_CLS(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_CLS" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_RET(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_RET" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_JP_ADDR(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_JP_ADDR" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_CALL_ADDR(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_CALL_ADDR" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_SE_VX_KK(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SE_VX_KK" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_SNE_VX_KK(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SNE_VX_KK" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_SE_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SE_VX_VY" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_LD_VX_KK(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_VX_KK" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_ADD_VX_KK(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_ADD_VX_KK" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_LD_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_VX_VY" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_OR_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_OR_VX_VY" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_AND_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_AND_VX_VY" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_XOR_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_XOR_VX_VY" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_ADD_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_ADD_VX_VY" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_SUB_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SUB_VX_VY" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_SHR_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SHR_VX_VY" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_SUBN_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SUBN_VX_VY" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_SHL_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SHL_VX_VY" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_SNE_VX_VY(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SNE_VX_VY" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_LD_I_ADDR(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_I_ADDR" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_JP_V0_ADDR(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_JP_V0_ADDR" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_RND_VX_KK(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_RND_VX_KK" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_DRW_VX_VY_N(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_DRW_VX_VY_N" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_SKP_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SKP_VX" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_SKNP_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_SKNP_VX" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_LD_VX_DT(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_VX_DT" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_LD_VX_K(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_VX_K" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_LD_DT_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_DT_VX" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_LD_ST_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_ST_VX" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_ADD_I_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_ADD_I_VX" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_LD_F_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_F_VX" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_LD_B_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_B_VX" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_LD_I_VX(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_I_VX" << std::endl;
    return false;
}

//--------------------------------------------------------------------------------
bool CPU::Execute_LD_VX_I(const Instruction& instruction)
{
    std::cout << "[UNIMPLEMENTED] Execute_LD_VX_I" << std::endl;
    return false;
}
