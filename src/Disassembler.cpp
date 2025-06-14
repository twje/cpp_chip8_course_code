#include "Disassembler.h"

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "InstructionSet.h"

//--------------------------------------------------------------------------------
std::optional<Instruction> Disassembler::TryGetInstruction(uint16_t opcode, uint16_t address)
{
    for (const InstructionDef& instructionDef : INSTRUCTION_SET)
    {
        if ((opcode & instructionDef.mMask) == instructionDef.mPattern)
        {
            // Parse opcode arguments
            std::vector<uint16_t> arguments;
            for (const InstructionArgDef& argDef : instructionDef.mArgs)
            {
                uint16_t value = (opcode & argDef.mMask) >> argDef.mShift;
                arguments.push_back(value);
            }

            return Instruction{ instructionDef.mId, std::move(arguments), address };
        }
    }

    // No matching instruction found — possibly invalid opcode
    return std::nullopt;
}