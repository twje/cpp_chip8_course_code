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
enum class OperandLabel
{
    NNN, // 12-bit address
    KK,  // 8-bit immediate
    N,   // 4-bit nibble
    X,   // register X
    Y,   // register Y
    None
};

//--------------------------------------------------------------------------------
struct OperandDef
{
    uint16_t mMask;
    uint8_t mShift;
    OperandLabel mLabel;
};

//--------------------------------------------------------------------------------
struct OpcodeFormatDef
{
    OpcodeId mOpcodeId;
    uint16_t mMask;
    uint16_t mPattern;
    std::vector<OperandDef> mOperands;
};

//--------------------------------------------------------------------------------
extern const std::vector<OpcodeFormatDef> OPCODE_FORMAT_TABLE;