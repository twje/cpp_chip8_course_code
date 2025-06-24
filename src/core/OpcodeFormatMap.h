#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "OpcodeId.h"

// System
#include <unordered_map>
#include <cstdint>
#include <string>

// Enums
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
struct OperandFormat
{
    uint16_t mMask;
    uint8_t mShift;
    OperandLabel mLabel;
};

//--------------------------------------------------------------------------------
struct OpcodeFormat
{
    uint16_t mMask;
    uint16_t mPattern;
    std::vector<OperandFormat> mOperands;
};

// Constants
//--------------------------------------------------------------------------------
extern const std::unordered_map<OpcodeId, OpcodeFormat> OPCODE_FORMAT_MAP;

// Functions
//--------------------------------------------------------------------------------
std::string OperandLabelToString(OperandLabel label);
