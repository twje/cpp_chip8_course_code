#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "OpcodeFormatTable.h"
#include "OpcodeId.h"

// System
#include <vector>
#include <ostream>

//--------------------------------------------------------------------------------
struct Instruction
{
    Instruction(uint16_t address, uint16_t opcode)
        : mOpcodeId(OpcodeId::UNASSIGNED)
        , mAddress(address)
        , mOpcode(opcode)
    { }

    bool MatchesPattern(const OpcodeFormatDef& def) const
    {
        // Apply the mask to isolate pattern-relevant bits, then compare to expected pattern
        // e.g. (0x8123 & 0xF00F) == 0x8003 for XOR_VX_VY
        // See http://devernay.free.fr/hacks/chip8/C8TECH10.HTM (3.1 - Standard Chip-8 Instructions)
        return (mOpcode & def.mMask) == def.mPattern;
    }

    void DecodeFrom(const OpcodeFormatDef& def)
    {
        mOpcodeId = def.mOpcodeId;

        mArguments.clear();
        for (const auto& operandDef : def.mOperands)
        {
            uint16_t value = (mOpcode & operandDef.mMask) >> operandDef.mShift;
            mArguments.push_back(value);
        }
    }

    OpcodeId GetOpcodeId() const { return mOpcodeId; }
    std::string GetPatternIdString() const { return GetOpcodePatternString(mOpcodeId); }

    uint16_t GetAddress() const { return mAddress; }
    uint16_t GetOpcode() const { return mOpcode; }

    template <typename T>
    T GetArgument(size_t index) const
    {
        return static_cast<T>(mArguments.at(index));
    }

private:
    OpcodeId mOpcodeId;
    std::vector<uint16_t> mArguments;

    // Core decode metadata
    uint16_t mAddress; // Memory address the opcode was read from.
    uint16_t mOpcode;
};

//--------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Instruction& instruction);