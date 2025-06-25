#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "OpcodeFormatMap.h"
#include "OpcodeId.h"

// System
#include <vector>
#include <ostream>
#include <string>
#include <optional>

//--------------------------------------------------------------------------------
// Instruction decode status
//--------------------------------------------------------------------------------
enum class DecodeStatus
{
    NOT_DECODED,
    OK,
    UNKNOWN_OPCODE,
};

//--------------------------------------------------------------------------------
// Instruction execution status
//--------------------------------------------------------------------------------
enum class ExecutionStatus
{
    Executed,
    DecodeError,
    NotImplemented,
    MissingHandler
};

//--------------------------------------------------------------------------------
// UI / Display metadata (used by debug panels and logs)
//--------------------------------------------------------------------------------
struct OperandInfo
{
    std::string mLabel;
    uint16_t mValue = 0;
};

//--------------------------------------------------------------------------------
struct InstructionInfo
{
    uint16_t mAddress = 0;
    uint16_t mOpcode = 0;
    std::string mPattern;               // e.g. "8xy1"
    std::string mMnemonic;              // e.g. "OR"
    std::vector<OperandInfo> mOperands; // e.g. [ {X, 1}, {Y, 2} ]
    DecodeStatus mDecodeStatus = DecodeStatus::NOT_DECODED;

    size_t mPreviewCycle = 0; // Metadata: Emulator cycle of preview
};

//--------------------------------------------------------------------------------
// Instruction data model
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

//--------------------------------------------------------------------------------
// Instruction execution result (returned by Emulator::Step)
//--------------------------------------------------------------------------------
struct StepResult
{
    std::optional<Instruction> mInstruction;
    ExecutionStatus mStatus;
    size_t mCycle = 0; // Cycle at which this instruction was executed
};
