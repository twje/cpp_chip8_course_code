#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "OpcodePatternTable.h"
#include "OpcodePatternId.h"

// System
#include <vector>
#include <ostream>

//--------------------------------------------------------------------------------
struct Instruction
{
    Instruction(uint16_t address, uint16_t opcode)
        : mPatternId(OpcodePatternId::UNASSIGNED)
        , mAddress(address)
        , mOpcode(opcode)
    { }

    bool MatchesPattern(const OpcodePatternDef& def) const
    {
        // Apply the mask to isolate pattern-relevant bits, then compare to expected pattern
        // e.g. (0x8123 & 0xF00F) == 0x8003 for XOR_VX_VY
        // See http://devernay.free.fr/hacks/chip8/C8TECH10.HTM (3.1 - Standard Chip-8 Instructions)
        return (mOpcode & def.mMask) == def.mPattern;
    }

    void DecodeFrom(const OpcodePatternDef& def)
    {
        mPatternId = def.mPatternId;

        mArguments.clear();
        for (const auto& argDef : def.mArgs)
        {
            uint16_t value = (mOpcode & argDef.mMask) >> argDef.mShift;
            mArguments.push_back(value);
        }
    }

    OpcodePatternId GetPatternId() const { return mPatternId; }
    std::string GetPatternIdString() const { return GetOpcodePatternString(mPatternId); }

    uint16_t GetAddress() const { return mAddress; }
    uint16_t GetOpcode() const { return mOpcode; }

    template <typename T>
    T GetArgument(size_t index) const
    {
        return static_cast<T>(mArguments.at(index));
    }

private:
    OpcodePatternId mPatternId;
    std::vector<uint16_t> mArguments;

    // Core decode metadata
    uint16_t mAddress; // Memory address the opcode was read from.
    uint16_t mOpcode;
};

//--------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Instruction& instruction);