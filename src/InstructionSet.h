#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "OpcodePatternId.h"

// System
#include <vector>
#include <cstdint>
#include <string>

//--------------------------------------------------------------------------------
struct InstructionArgDef
{
    uint16_t mMask;
    uint8_t mShift;
};

//--------------------------------------------------------------------------------
struct InstructionDef
{
    OpcodePatternId mOpcodePatternId;
    uint16_t mMask;
    uint16_t mOpcodePattern;
    std::vector<InstructionArgDef> mArgs;
};

//--------------------------------------------------------------------------------
extern const std::vector<InstructionDef> INSTRUCTION_SET;