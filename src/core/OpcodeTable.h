#pragma once

// Includes
//------------------------------------------------------------------------------
// Emulator
#include "OpcodeId.h"

// System
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

//------------------------------------------------------------------------------
struct OperandSpec
{
    uint16_t mMask;
    uint8_t mShift;
    const char* mLabel; // e.g. "x", "nnn"
};

//------------------------------------------------------------------------------
struct OpcodeSpec
{
    uint16_t mMask;
    uint16_t mPattern;
    const char* mPatternStr; // e.g. "8xy1"
    const char* mMnemonic;   // e.g. "OR"
    std::vector<OperandSpec> mOperands;
};

//------------------------------------------------------------------------------
extern const std::unordered_map<OpcodeId, OpcodeSpec> OPCODE_TABLE;