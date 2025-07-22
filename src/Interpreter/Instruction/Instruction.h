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
    static constexpr int OPERAND_NNN = 0;
    static constexpr int OPERAND_KK = 1;
    static constexpr int OPERAND_X = 0;
    static constexpr int OPERAND_Y = 1;
    static constexpr int OPERAND_N = 2;

	static constexpr uint16_t MASK_NNN = 0x0FFF; // NNN (12 bits)
	static constexpr uint8_t  MASK_KK = 0xFF;    // Byte (8 bits)
    static constexpr uint8_t  MASK_N = 0x0F;     // Nibble (4 bits)
	static constexpr uint8_t  MASK_XY = 0x0F;    // Nibble (4 bits)

public:
    Instruction();
    Instruction(const OpcodeId opcodeId, const std::vector<uint16_t>& operands);

	OpcodeId GetOpcodeId() const { return mOpcodeId; }	
	bool IsValid() const { return mOpcodeId != OpcodeId::UNASSIGNED; }
    size_t OperandCount() const { return mOperands.size(); }

    // Operand accessors
    uint16_t GetOperandNNN() const { return mOperands.at(OPERAND_NNN) & MASK_NNN; }
    uint8_t GetOperandKK() const { return static_cast<uint8_t>(mOperands.at(OPERAND_KK) & MASK_KK); }
    uint8_t GetOperandN() const { return static_cast<uint8_t>(mOperands.at(OPERAND_N) & MASK_N); }
    size_t GetOperandX() const { return static_cast<size_t>(mOperands.at(OPERAND_X) & MASK_XY); }
    size_t GetOperandY() const { return static_cast<size_t>(mOperands.at(OPERAND_Y) & MASK_XY); }

private:
    OpcodeId mOpcodeId;
    std::vector<uint16_t> mOperands;
};