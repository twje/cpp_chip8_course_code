#include "CPU.h"

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Config.h"
#include "DelayTimer.h"
#include "Display.h"
#include "Instruction.h"
#include "Keypad.h"
#include "RAM.h"
#include "SoundTimer.h"

// System
#include <cassert>

//--------------------------------------------------------------------------------
CPU::CPU(Bus& bus)
    : mBus{ bus }
{ 
    mState.mPC = PROGRAM_START_ADDRESS;
}

//--------------------------------------------------------------------------------
void CPU::Reset()
{
    mState.mPC = PROGRAM_START_ADDRESS;
    mState.mI = 0;
    mState.mV.fill(0);
    mState.mSP = 0;
    mState.mStack.fill(0);
    mState.mDelayTimer = 0;
    mState.mSoundTimer = 0;
}

// CHIP-8 stores opcodes as two consecutive bytes in big-endian format.
// Read and combine the two bytes into a single 16-bit opcode.
//--------------------------------------------------------------------------------
uint16_t CPU::PeekNextOpcode() const
{
    const uint16_t address = mState.mPC;
    assert(address % 2 == 0);
    assert(address >= PROGRAM_START_ADDRESS && address + 1 < RAM_SIZE);

    const uint8_t hByte = mBus.mRAM.Read(address);
    const uint8_t lByte = mBus.mRAM.Read(address + 1);

    const uint16_t opcode = (static_cast<uint16_t>(hByte) << 8) | lByte;
    
    return opcode;
}

//--------------------------------------------------------------------------------
uint16_t CPU::FetchOpcode()
{
    uint16_t opcode = PeekNextOpcode();

    // Advance PC early (some instructions override it)
    mState.mPC += 2;

    return opcode;
}

//--------------------------------------------------------------------------------
Instruction CPU::Decode(uint16_t opcode) const
{
    for (const auto& [opcodeId, opcodeSpec] : OPCODE_TABLE)
    {
        // Apply the mask to isolate pattern-relevant bits, then compare to expected pattern
        // e.g. (0x8123 & 0xF00F) == 0x8003 for XOR_VX_VY
        if ((opcode & opcodeSpec.mMask) == opcodeSpec.mPattern)
        {
            // Parse opcode operands
            std::vector<uint16_t> operands;            
            for (const auto& operandSpec : opcodeSpec.mOperands)
            {
                uint16_t value = (opcode & operandSpec.mMask) >> operandSpec.mShift;
                operands.push_back(value);
            }
        
            return { opcodeId, operands };
        }
    }

    return { };
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute(const Instruction& instruction)
{
    switch (instruction.mOpcodeId)
    {
        case OpcodeId::SYS_ADDR:    return Execute_0nnn_SYS_ADDR(instruction);
        case OpcodeId::CLS:         return Execute_00E0_CLS(instruction);
        case OpcodeId::RET:         return Execute_00EE_RET(instruction);
        case OpcodeId::JP_ADDR:     return Execute_1nnn_JP_ADDR(instruction);
        case OpcodeId::CALL_ADDR:   return Execute_2nnn_CALL_ADDR(instruction);
        case OpcodeId::SE_VX_KK:    return Execute_3xkk_SE_VX_KK(instruction);
        case OpcodeId::SNE_VX_KK:   return Execute_4xkk_SNE_VX_KK(instruction);
        case OpcodeId::SE_VX_VY:    return Execute_5xy0_SE_VX_VY(instruction);
        case OpcodeId::LD_VX_KK:    return Execute_6xkk_LD_VX_KK(instruction);
        case OpcodeId::ADD_VX_KK:   return Execute_7xkk_ADD_VX_KK(instruction);
        case OpcodeId::LD_VX_VY:    return Execute_8xy0_LD_VX_VY(instruction);
        case OpcodeId::OR_VX_VY:    return Execute_8xy1_OR_VX_VY(instruction);
        case OpcodeId::AND_VX_VY:   return Execute_8xy2_AND_VX_VY(instruction);
        case OpcodeId::XOR_VX_VY:   return Execute_8xy3_XOR_VX_VY(instruction);
        case OpcodeId::ADD_VX_VY:   return Execute_8xy4_ADD_VX_VY(instruction);
        case OpcodeId::SUB_VX_VY:   return Execute_8xy5_SUB_VX_VY(instruction);
        case OpcodeId::SHR_VX_VY:   return Execute_8xy6_SHR_VX_VY(instruction);
        case OpcodeId::SUBN_VX_VY:  return Execute_8xy7_SUBN_VX_VY(instruction);
        case OpcodeId::SHL_VX_VY:   return Execute_8xyE_SHL_VX_VY(instruction);
        case OpcodeId::SNE_VX_VY:   return Execute_9xy0_SNE_VX_VY(instruction);
        case OpcodeId::LD_I_ADDR:   return Execute_Annn_LD_I_ADDR(instruction);
        case OpcodeId::JP_V0_ADDR:  return Execute_Bnnn_JP_V0_ADDR(instruction);
        case OpcodeId::RND_VX_KK:   return Execute_Cxkk_RND_VX_KK(instruction);
        case OpcodeId::DRW_VX_VY_N: return Execute_Dxyn_DRW_VX_VY_N(instruction);
        case OpcodeId::SKP_VX:      return Execute_Ex9E_SKP_VX(instruction);
        case OpcodeId::SKNP_VX:     return Execute_ExA1_SKNP_VX(instruction);
        case OpcodeId::LD_VX_DT:    return Execute_Fx07_LD_VX_DT(instruction);
        case OpcodeId::LD_VX_K:     return Execute_Fx0A_LD_VX_K(instruction);
        case OpcodeId::LD_DT_VX:    return Execute_Fx15_LD_DT_VX(instruction);
        case OpcodeId::LD_ST_VX:    return Execute_Fx18_LD_ST_VX(instruction);
        case OpcodeId::ADD_I_VX:    return Execute_Fx1E_ADD_I_VX(instruction);
        case OpcodeId::LD_F_VX:     return Execute_Fx29_LD_F_VX(instruction);
        case OpcodeId::LD_B_VX:     return Execute_Fx33_LD_B_VX(instruction);
        case OpcodeId::LD_I_VX:     return Execute_Fx55_LD_I_VX(instruction);
        case OpcodeId::LD_VX_I:     return Execute_Fx65_LD_VX_I(instruction);
    }

    return ExecutionStatus::MissingHandler;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_0nnn_SYS_ADDR(const Instruction&)
{
    // Legacy SYS instruction (0nnn); ignored in modern interpreters.
    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_00E0_CLS(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_00EE_RET(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_1nnn_JP_ADDR(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_2nnn_CALL_ADDR(const Instruction& instruction)
{
    const uint16_t address = instruction.GetOperand<uint16_t>(0);

    mState.mStack[mState.mSP] = mState.mPC;
    mState.mSP++;
    mState.mPC = address;
 
    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_3xkk_SE_VX_KK(const Instruction& instruction)
{
    const size_t vxIndex = instruction.GetOperand<size_t>(0);
    const uint8_t value = instruction.GetOperand<uint8_t>(1);
    const uint8_t vx = mState.mV.at(vxIndex);

    if (vx == value)
    {
        mState.mPC += INSTRUCTION_SIZE;
    }

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_4xkk_SNE_VX_KK(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_5xy0_SE_VX_VY(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_6xkk_LD_VX_KK(const Instruction& instruction)
{
    const size_t vxIndex = instruction.GetOperand<size_t>(0);
    const uint8_t value = instruction.GetOperand<uint8_t>(1);
    mState.mV[vxIndex] = value;

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_7xkk_ADD_VX_KK(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy0_LD_VX_VY(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy1_OR_VX_VY(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy2_AND_VX_VY(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy3_XOR_VX_VY(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy4_ADD_VX_VY(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy5_SUB_VX_VY(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy6_SHR_VX_VY(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy7_SUBN_VX_VY(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xyE_SHL_VX_VY(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_9xy0_SNE_VX_VY(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Annn_LD_I_ADDR(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Bnnn_JP_V0_ADDR(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Cxkk_RND_VX_KK(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Dxyn_DRW_VX_VY_N(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Ex9E_SKP_VX(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_ExA1_SKNP_VX(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx07_LD_VX_DT(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx0A_LD_VX_K(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx15_LD_DT_VX(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx18_LD_ST_VX(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx1E_ADD_I_VX(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx29_LD_F_VX(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx33_LD_B_VX(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx55_LD_I_VX(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx65_LD_VX_I(const Instruction&)
{
    return ExecutionStatus::NotImplemented;
}
