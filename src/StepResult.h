#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Instruction.h" 

//--------------------------------------------------------------------------------
enum class StepResultStatus
{
    Ignored,
    Executed,
    DecodeError,
    Unimplemented,
};

//--------------------------------------------------------------------------------
struct StepResult
{
    StepResultStatus mStatus;
    Instruction mInstruction;
};
