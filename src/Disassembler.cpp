#include "Disassembler.h"

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Instruction.h"
#include "InstructionSet.h"

//--------------------------------------------------------------------------------
bool Disassembler::Decode(Instruction& outInstruction)
{
    for (const InstructionDef& instructionDef : INSTRUCTION_SET)
    {
        if (outInstruction.MatchesPattern(instructionDef))
        {
            outInstruction.ApplyDefinition(instructionDef);
            return true;
        }
    }

    // No matching instruction found — possibly invalid opcode
    return false;
}