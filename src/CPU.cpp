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

//--------------------------------------------------------------------------------
StepResult CPU::Step()
{
    const uint16_t address = mPC;
    const uint16_t opcode = Fetch(address);

    // Advance PC early (some instructions override it)
    mPC += 2;

    // Ignore 0nnn (RCA 1802 call) — obsolete on modern interpreters
    if ((opcode & 0xF000) == 0x0000)
    {
        return StepResult::Ignored();
    }

    // Decode
    std::optional<Instruction> instructionOpt = mDisassembler.TryGetInstruction(opcode, address);
    if (!instructionOpt)
    {
        return StepResult::DecodeError(opcode, address);
    }

    bool isImplemented = Execute(*instructionOpt);
    if (isImplemented)
    {
        return StepResult::Executed(*instructionOpt);
    }

    return StepResult::Unimplemented(*instructionOpt);
}

//--------------------------------------------------------------------------------
uint16_t CPU::Fetch(uint16_t address)
{
    assert(address % 2 == 0);
    const uint8_t hByte = mBus.mRAM.Read(address);
    const uint8_t lByte = mBus.mRAM.Read(address + 1);

    // CHIP-8 opcodes are stored big-endian: high byte first in memory.
    return (static_cast<uint16_t>(hByte) << 8) | lByte;
}

//--------------------------------------------------------------------------------
bool CPU::Execute(const Instruction& instruction)
{
    switch (instruction.mOpcodeId) // TODO: think about changing to ID?
    {
        case OpcodeId::CLS:           return Execute_CLS(instruction);
        case OpcodeId::RET:           return Execute_RET(instruction);
        case OpcodeId::JP_ADDR:       return Execute_JP_ADDR(instruction);
        case OpcodeId::CALL_ADDR:     return Execute_CALL_ADDR(instruction);
        case OpcodeId::SE_VX_KK:      return Execute_SE_VX_KK(instruction);
        case OpcodeId::SNE_VX_KK:     return Execute_SNE_VX_KK(instruction);
        case OpcodeId::SE_VX_VY:      return Execute_SE_VX_VY(instruction);
        case OpcodeId::LD_VX_KK:      return Execute_LD_VX_KK(instruction);
        case OpcodeId::ADD_VX_KK:     return Execute_ADD_VX_KK(instruction);
        case OpcodeId::LD_VX_VY:      return Execute_LD_VX_VY(instruction);
        case OpcodeId::OR_VX_VY:      return Execute_OR_VX_VY(instruction);
        case OpcodeId::AND_VX_VY:     return Execute_AND_VX_VY(instruction);
        case OpcodeId::XOR_VX_VY:     return Execute_XOR_VX_VY(instruction);
        case OpcodeId::ADD_VX_VY:     return Execute_ADD_VX_VY(instruction);
        case OpcodeId::SUB_VX_VY:     return Execute_SUB_VX_VY(instruction);
        case OpcodeId::SHR_VX_VY:     return Execute_SHR_VX_VY(instruction);
        case OpcodeId::SUBN_VX_VY:    return Execute_SUBN_VX_VY(instruction);
        case OpcodeId::SHL_VX_VY:     return Execute_SHL_VX_VY(instruction);
        case OpcodeId::SNE_VX_VY:     return Execute_SNE_VX_VY(instruction);
        case OpcodeId::LD_I_ADDR:     return Execute_LD_I_ADDR(instruction);
        case OpcodeId::JP_V0_ADDR:    return Execute_JP_V0_ADDR(instruction);
        case OpcodeId::RND_VX_KK:     return Execute_RND_VX_KK(instruction);
        case OpcodeId::DRW_VX_VY_N:   return Execute_DRW_VX_VY_N(instruction);
        case OpcodeId::SKP_VX:        return Execute_SKP_VX(instruction);
        case OpcodeId::SKNP_VX:       return Execute_SKNP_VX(instruction);
        case OpcodeId::LD_VX_DT:      return Execute_LD_VX_DT(instruction);
        case OpcodeId::LD_VX_K:       return Execute_LD_VX_K(instruction);
        case OpcodeId::LD_DT_VX:      return Execute_LD_DT_VX(instruction);
        case OpcodeId::LD_ST_VX:      return Execute_LD_ST_VX(instruction);
        case OpcodeId::ADD_I_VX:      return Execute_ADD_I_VX(instruction);
        case OpcodeId::LD_F_VX:       return Execute_LD_F_VX(instruction);
        case OpcodeId::LD_B_VX:       return Execute_LD_B_VX(instruction);
        case OpcodeId::LD_I_VX:       return Execute_LD_I_VX(instruction);
        case OpcodeId::LD_VX_I:       return Execute_LD_VX_I(instruction);
    }

    std::cerr << "Unimplemented opcode: " + OpCodeIdToString(instruction.mOpcodeId) << std::endl;
    return false;
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
