#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "OpcodeId.h"

// System
#include <vector>

//--------------------------------------------------------------------------------
class Instruction
{
public:
    Instruction(OpcodeId id, std::vector<uint16_t>&& arguments);

    OpcodeId GetId() const { return mId; }
    const std::vector<uint16_t>& GetArguments() const { return mArguments; }

private:
    OpcodeId mId;
    std::vector<uint16_t> mArguments;
};
