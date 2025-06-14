#include "Instruction.h"

//--------------------------------------------------------------------------------
Instruction::Instruction(OpcodeId id, std::vector<uint16_t>&& arguments)
    : mId(id)
    , mArguments(std::move(arguments))
{ }