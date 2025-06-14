#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "OpcodeId.h"

// System
#include <vector>
#include <ostream>

//--------------------------------------------------------------------------------
struct Instruction
{
    OpcodeId mOpcodeId;
    std::vector<uint16_t> mArguments;
    uint16_t mAddress;    
};

//--------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Instruction& instrction);