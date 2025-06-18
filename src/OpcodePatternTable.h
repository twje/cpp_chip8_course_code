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
struct OpcodeArgDef
{
    uint16_t mMask;
    uint8_t mShift;
};

//--------------------------------------------------------------------------------
struct OpcodePatternDef
{
    OpcodePatternId mPatternId;
    uint16_t mMask;
    uint16_t mPattern;
    std::vector<OpcodeArgDef> mArgs;
};

//--------------------------------------------------------------------------------
extern const std::vector<OpcodePatternDef> OPCODE_PATTERN_TABLE;