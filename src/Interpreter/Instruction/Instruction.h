#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interpreter/Instruction/OpcodeId.h"

// System
#include <string>
#include <vector>
#include <cassert>

//--------------------------------------------------------------------------------
class Instruction
{
public:
    Instruction();
    Instruction(const OpcodeId opcodeId, const std::vector<uint16_t>& operands);

	OpcodeId GetOpcodeId() const { return mOpcodeId; }
	const std::vector<uint16_t>& GetOperands() const { return mOperands; }
	bool IsValid() const { return mOpcodeId != OpcodeId::UNASSIGNED; }

    // Operand accessors based on CHIP-8 conventions
    uint8_t GetOperandKK() const { return GetOperand<uint8_t>(1); }
    uint8_t GetOperandN() const { return GetOperand<uint8_t>(2); }
    uint16_t GetOperandNNN() const { return GetOperand<uint16_t>(0); }
    size_t GetOperandX() const { return GetOperand<size_t>(0); }
    size_t GetOperandY() const { return GetOperand<size_t>(1); }

private:
    // General-purpose typed operand accessor
    template <typename T>
    T GetOperand(size_t index) const
    {
        assert(index < mOperands.size());
        return static_cast<T>(mOperands.at(index));
    }

    OpcodeId mOpcodeId;
    std::vector<uint16_t> mOperands;
};