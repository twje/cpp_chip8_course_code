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
        if ((outInstruction.mOpcode & instructionDef.mMask) == instructionDef.mPattern)
        {
            outInstruction.mInstructionPatternId = instructionDef.mId;  // TODO: think about names

            // Parse opcode arguments
            std::vector<uint16_t> arguments;
            for (const InstructionArgDef& argDef : instructionDef.mArgs)
            {
                uint16_t value = (outInstruction.mOpcode & argDef.mMask) >> argDef.mShift;
                outInstruction.mArguments.push_back(value);                
            }

            return true;
        }
    }

    // No matching instruction found — possibly invalid opcode
    return false;
}