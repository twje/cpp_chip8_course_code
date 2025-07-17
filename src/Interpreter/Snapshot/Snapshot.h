#pragma once

// Includes
//--------------------------------------------------------------------------------
// Intepreter
#include "Interpreter/Hardware/CPUState.h"

// System
#include <string>
#include <vector>

// Forward Declarations
//--------------------------------------------------------------------------------
class Instruction;

// Operand info (part of instruction decoding)
//--------------------------------------------------------------------------------
struct OperandInfo
{
    std::string mLabel;
    uint16_t mValue = 0;
};

//--------------------------------------------------------------------------------
struct Snapshot
{
    // Instruction information
    uint16_t mAddress = 0;                     // Address of instruction
    uint16_t mOpcode = 0;                      // Raw opcode
    std::string mPattern;                      // Instruction pattern, e.g., "8xy1"
    std::string mMnemonic;                     // Instruction mnemonic, e.g., "OR"
    std::vector<OperandInfo> mOperands;        // Operands with labels and values

    // Decode metadata
    bool mDecodeSucceeded = false;             // Whether decoding succeeded
    size_t mCycleCount = 0;                    // Interpreter cycle count at snapshot

    // CPU state snapshot (full CPU registers, etc.)
    CPUState mCPUState;
};