#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Instruction.h"

// System
#include <optional>

//--------------------------------------------------------------------------------
enum class ExecutionStatus
{
    Executed,
    DecodeError,
    NotImplemented,
    MissingHandler
};

//--------------------------------------------------------------------------------
struct StepResult 
{
    std::optional<Instruction> mInstruction;
    ExecutionStatus mStatus;
    size_t mCycle = 0; // Cycle at which this instruction was executed
};