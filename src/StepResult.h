#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Instruction.h" 

// System
#include <optional>
#include <cstdint>

//--------------------------------------------------------------------------------
struct StepResult
{
    enum class Status
    {
        Ignored,
        Executed,
        DecodeError,
        Unimplemented,
    };
    
    static StepResult Ignored()
    {
        return { Status::Ignored, std::nullopt, std::nullopt, std::nullopt };
    }

    static StepResult Executed(const Instruction& instruction)
    {
        return { Status::Executed, instruction, std::nullopt, std::nullopt };
    }

    static StepResult DecodeError(uint16_t opcode, uint16_t address)
    {
        return { Status::DecodeError, std::nullopt, opcode, address };
    }

    static StepResult Unimplemented(const Instruction& instruction)
    {
        return { Status::Unimplemented, instruction, std::nullopt, std::nullopt };
    }

    static StepResult Unimplemented(uint16_t opcode, uint16_t address)
    {
        return { Status::Unimplemented, std::nullopt, opcode, address };
    }

    Status mStatus;
    std::optional<Instruction> mInstruction;
    std::optional<uint16_t> mOpcode;
    std::optional<uint16_t> mAddress;
};
