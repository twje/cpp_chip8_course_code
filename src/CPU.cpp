#include "CPU.h"

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Instruction.h"

// System
#include <optional>
#include <iostream>
#include <cassert>

// Macros
//--------------------------------------------------------------------------------
#define DEFINE_OPCODE_HANDLER(name) \
    void CPU::Execute_##name(const Instruction& instruction) { \
        std::cout << "[UNIMPLEMENTED] Execute_" #name << std::endl; \
    }

//--------------------------------------------------------------------------------
CPU::CPU(Bus& bus)
	: mBus{ bus }
    , mPC(PROGRAM_START_ADDRESS)
{ }

//--------------------------------------------------------------------------------
void CPU::Step()
{
    const uint16_t address = mPC;
    const uint16_t opcode = Fetch(address);

    // Advance PC early (some instructions override it)
    mPC += 2;

    // Ignore 0nnn (RCA 1802 call) — obsolete on modern interpreters
    if ((opcode & 0xF000) == 0x0000)
    {
        return;
    }

    // Decode
    std::optional<Instruction> instructionOpt = mDisassembler.TryGetInstruction(opcode, address);
    if (!instructionOpt.has_value())
    {
        throw std::runtime_error("Unknown or invalid opcode: " + std::to_string(opcode));
    }

    Execute(*instructionOpt);
}

//--------------------------------------------------------------------------------
uint16_t CPU::Fetch(uint16_t address)
{
    assert(address % 2 == 0);
    const uint8_t hByte = mBus.Read8(address);
    const uint8_t lByte = mBus.Read8(address + 1);

    // CHIP-8 opcodes are stored big-endian: high byte first in memory.
    return (static_cast<uint16_t>(hByte) << 8) | lByte;
}

//--------------------------------------------------------------------------------
void CPU::Execute(const Instruction& instruction)
{
    std::cout << instruction << std::endl;

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

        default:
            throw std::runtime_error("Unimplemented opcode: " + OpCodeIdToString(instruction.mOpcodeId));
    }
}

DEFINE_OPCODE_HANDLER(CLS)
DEFINE_OPCODE_HANDLER(RET)
DEFINE_OPCODE_HANDLER(JP_ADDR)
DEFINE_OPCODE_HANDLER(CALL_ADDR)
DEFINE_OPCODE_HANDLER(SE_VX_KK)
DEFINE_OPCODE_HANDLER(SNE_VX_KK)
DEFINE_OPCODE_HANDLER(SE_VX_VY)
DEFINE_OPCODE_HANDLER(LD_VX_KK)
DEFINE_OPCODE_HANDLER(ADD_VX_KK)
DEFINE_OPCODE_HANDLER(LD_VX_VY)
DEFINE_OPCODE_HANDLER(OR_VX_VY)
DEFINE_OPCODE_HANDLER(AND_VX_VY)
DEFINE_OPCODE_HANDLER(XOR_VX_VY)
DEFINE_OPCODE_HANDLER(ADD_VX_VY)
DEFINE_OPCODE_HANDLER(SUB_VX_VY)
DEFINE_OPCODE_HANDLER(SHR_VX_VY)
DEFINE_OPCODE_HANDLER(SUBN_VX_VY)
DEFINE_OPCODE_HANDLER(SHL_VX_VY)
DEFINE_OPCODE_HANDLER(SNE_VX_VY)
DEFINE_OPCODE_HANDLER(LD_I_ADDR)
DEFINE_OPCODE_HANDLER(JP_V0_ADDR)
DEFINE_OPCODE_HANDLER(RND_VX_KK)
DEFINE_OPCODE_HANDLER(DRW_VX_VY_N)
DEFINE_OPCODE_HANDLER(SKP_VX)
DEFINE_OPCODE_HANDLER(SKNP_VX)
DEFINE_OPCODE_HANDLER(LD_VX_DT)
DEFINE_OPCODE_HANDLER(LD_VX_K)
DEFINE_OPCODE_HANDLER(LD_DT_VX)
DEFINE_OPCODE_HANDLER(LD_ST_VX)
DEFINE_OPCODE_HANDLER(ADD_I_VX)
DEFINE_OPCODE_HANDLER(LD_F_VX)
DEFINE_OPCODE_HANDLER(LD_B_VX)
DEFINE_OPCODE_HANDLER(LD_I_VX)
DEFINE_OPCODE_HANDLER(LD_VX_I)