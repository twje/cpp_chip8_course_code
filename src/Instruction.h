#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "InstructionPatternId.h"

// System
#include <vector>
#include <ostream>

//--------------------------------------------------------------------------------
struct Instruction
{
    Instruction(uint16_t opcode, uint16_t address)
        : mInstructionPatternId(InstructionPatternId::UNASSIGNED)
        , mAddress(address)
        , mOpcode(opcode) 
    { }

    InstructionPatternId mInstructionPatternId;
    std::vector<uint16_t> mArguments;
    uint16_t mAddress;
    uint16_t mOpcode;
};

//--------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Instruction& instrction);