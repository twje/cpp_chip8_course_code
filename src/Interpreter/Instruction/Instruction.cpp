#include "Interpreter/Instruction/Instruction.h"

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interpreter/Instruction/OpcodeTable.h"

//--------------------------------------------------------------------------------
Instruction::Instruction()
    : mOpcodeId(OpcodeId::UNASSIGNED)
{ }

//--------------------------------------------------------------------------------
Instruction::Instruction(const OpcodeId opcodeId, const std::vector<uint16_t>& operands)
    : mOpcodeId(opcodeId)
    , mOperands(operands)
{
    assert(opcodeId != OpcodeId::UNASSIGNED);
    assert(operands.size() == OpcodeTable::Get(opcodeId).mOperands.size());
}