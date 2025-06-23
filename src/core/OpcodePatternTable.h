#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "OpcodeId.h"

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
    OpcodeId mOpcodeId;
    uint16_t mMask;
    uint16_t mPattern;
    std::vector<OpcodeArgDef> mArgs;
};

//--------------------------------------------------------------------------------
extern const std::vector<OpcodePatternDef> OPCODE_PATTERN_TABLE;