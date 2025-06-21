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
    , mProgramCounter{ PROGRAM_START_ADDRESS }
    , mIndexRegister{ 0 }
    , mRegisters{}        // std::array — zero-initialized
    , mStackPointer{ 0 }
    , mStack{}            // std::array — zero-initialized
    , mDelayTimer{ 0 }
    , mSoundTimer{ 0 }
{ }

//--------------------------------------------------------------------------------
void CPU::Reset()
{
    mProgramCounter = PROGRAM_START_ADDRESS;
    mIndexRegister = 0;
    mRegisters.fill(0);
    mStackPointer = 0;
    mStack.fill(0);
    mDelayTimer = 0;
    mSoundTimer = 0;
}

//--------------------------------------------------------------------------------
uint8_t CPU::GetRegisterValueAt(size_t index) const
{
    return mRegisters.at(index);
}

//--------------------------------------------------------------------------------
uint16_t CPU::GetStackValueAt(size_t index) const
{
    return mStack.at(index);
}

// CHIP-8 stores opcodes as two consecutive bytes in big-endian format.
// Read and combine the two bytes into a single 16-bit opcode.
//--------------------------------------------------------------------------------
Instruction CPU::Peek()
{
    const uint16_t address = mProgramCounter;
    assert(address % 2 == 0);

    const uint8_t hByte = mBus.mRAM.Read(address);
    const uint8_t lByte = mBus.mRAM.Read(address + 1);

    const uint16_t opcode = (static_cast<uint16_t>(hByte) << 8) | lByte;
    Instruction instruction(address, opcode);

    return instruction;
}

//--------------------------------------------------------------------------------
Instruction CPU::Fetch()
{
    Instruction instruction = Peek();

    // Advance PC early (some instructions override it)
    mProgramCounter += 2;

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
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_RET(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_JP_ADDR(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_CALL_ADDR(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SE_VX_KK(const Instruction& instruction)
{
    const size_t vxIndex = instruction.GetArgument<size_t>(0);
    const uint8_t value = instruction.GetArgument<uint8_t>(1);
    const uint8_t vx = mRegisters.at(vxIndex);

    // Skip next instruction if Vx == kk
    if (vx == value)
    {
        mProgramCounter += INSTRUCTION_SIZE;
    }

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SNE_VX_KK(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SE_VX_VY(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_VX_KK(const Instruction& instruction)
{
    const size_t vxIndex = instruction.GetArgument<size_t>(0);
    const uint8_t value = instruction.GetArgument<uint8_t>(1);
    mRegisters[vxIndex] = value;

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_ADD_VX_KK(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_VX_VY(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_OR_VX_VY(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_AND_VX_VY(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_XOR_VX_VY(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_ADD_VX_VY(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SUB_VX_VY(const Instruction& instruction)
{    
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SHR_VX_VY(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SUBN_VX_VY(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SHL_VX_VY(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SNE_VX_VY(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_I_ADDR(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_JP_V0_ADDR(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_RND_VX_KK(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_DRW_VX_VY_N(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SKP_VX(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_SKNP_VX(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_VX_DT(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_VX_K(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_DT_VX(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_ST_VX(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_ADD_I_VX(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_F_VX(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_B_VX(const Instruction& instruction)
{
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_I_VX(const Instruction& instruction)
{ 
    return ExecutionStatus::NotImplemented;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_LD_VX_I(const Instruction& instruction)
{    
    return ExecutionStatus::NotImplemented;
}