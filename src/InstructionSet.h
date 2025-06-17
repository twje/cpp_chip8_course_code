#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "InstructionPatternId.h"

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
    InstructionPatternId mId;
    uint16_t mMask;
    uint16_t mPattern;
    std::vector<InstructionArgDef> mArgs;
};

//--------------------------------------------------------------------------------
extern const std::vector<InstructionDef> INSTRUCTION_SET;