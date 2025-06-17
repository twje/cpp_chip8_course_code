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
    InstructionPatternId mInstructionPatternId;
    std::vector<uint16_t> mArguments;
    uint16_t mAddress;    
};

//--------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Instruction& instrction);