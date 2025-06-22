#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Instruction.h"

//--------------------------------------------------------------------------------
enum class ExecutionStatus
{
    Executed,
    DecodeError,
    NotImplemented,
    MissingHandler
};

//--------------------------------------------------------------------------------
enum class PeekStatus
{
    Valid,
    DecodeError
};

//--------------------------------------------------------------------------------
struct StepResult 
{
    Instruction mInstruction;
    ExecutionStatus mStatus;
};

//--------------------------------------------------------------------------------
struct PeekResult
{
    Instruction mInstruction;
    PeekStatus status;
};