#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "OpcodeId.h"
#include "OpcodeTable.h"

// System
#include <string>
#include <vector>

//--------------------------------------------------------------------------------
struct OperandInfo
{
    std::string mLabel;
    uint16_t mValue = 0;
};

// Metadata used for debug panels, logging, or UI preview
//--------------------------------------------------------------------------------
struct InstructionInfo
{
    uint16_t mAddress = 0;
    uint16_t mOpcode = 0;
    std::string mPattern;               // e.g. "8xy1"
    std::string mMnemonic;              // e.g. "OR"
    std::vector<OperandInfo> mOperands; // e.g. [ {X, 1}, {Y, 2} ]
    bool mDecodeSucceeded = false;

    size_t mPreviewCycle = 0; // Metadata: Emulator cycle of preview
};

// Runtime instruction with decoded operands, used for execution
//--------------------------------------------------------------------------------
struct Instruction
{
    OpcodeId mOpcodeId = OpcodeId::UNASSIGNED;
    std::vector<uint16_t> mOperands;

    template <typename T>
    T GetOperand(size_t index) const
    {
        return static_cast<T>(mOperands.at(index));
    }
};