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
    // Operand indices
    static constexpr size_t kOperandNNN = 0;
    static constexpr size_t kOperandKK = 1;
    static constexpr size_t kOperandX = 0;
    static constexpr size_t kOperandY = 1;
    static constexpr size_t kOperandN = 2;

    // Operand masks (for clarity only — Decode opcode already ensures correct bit widths).
    static constexpr uint16_t kMaskNNN = 0x0FFF; // 12-bit address
    static constexpr uint16_t kMaskKK = 0x00FF;  // 8-bit byte
    static constexpr uint16_t kMaskN = 0x000F;   // 4-bit nibble
    static constexpr uint16_t kMaskXY = 0x000F;  // 4-bit nibble (used for register indices)

public:
    Instruction();
    Instruction(const OpcodeId opcodeId, const std::vector<uint16_t>& operands);

    OpcodeId GetOpcodeId() const { return mOpcodeId; }
    bool IsValid() const { return mOpcodeId != OpcodeId::UNASSIGNED; }
    size_t OperandCount() const { return mOperands.size(); }

    // Operand accessors
    uint16_t GetOperandNNN() const { return GetMaskedOperand<uint16_t>(kOperandNNN, kMaskNNN); }
    uint8_t GetOperandKK() const { return GetMaskedOperand<uint8_t>(kOperandKK, kMaskKK); }
    uint8_t GetOperandN() const { return GetMaskedOperand<uint8_t>(kOperandN, kMaskN); }
    size_t  GetOperandX() const { return GetMaskedOperand<size_t>(kOperandX, kMaskXY); }
    size_t  GetOperandY() const { return GetMaskedOperand<size_t>(kOperandY, kMaskXY); }

private:
    template<typename T>
    T GetMaskedOperand(size_t index, uint16_t bitMask) const
    {
        return static_cast<T>(mOperands.at(index) & bitMask);
    }

    OpcodeId mOpcodeId;
    std::vector<uint16_t> mOperands;
};