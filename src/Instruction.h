#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "InstructionSet.h"
#include "OpcodePatternId.h"

// System
#include <vector>
#include <ostream>

//--------------------------------------------------------------------------------
struct Instruction
{    
public:
    struct DebugInfo
    {
        DebugInfo(uint16_t address, uint16_t opcode)
            : mAddress(address)
            , mOpcode(opcode)
            , mMask(0)
            , mOpcodePattern(0)
        { }

        uint16_t mAddress;
        uint16_t mOpcode;

        // Populated during successful decode
        uint16_t mMask;
        uint16_t mOpcodePattern;
    };

    Instruction(uint16_t address, uint16_t opcode)
        : mOpcodePatternId(OpcodePatternId::UNASSIGNED)
        , mDebug(address, opcode)
    { }

    bool MatchesPattern(const InstructionDef& def) const
    {
        if ((mDebug.mOpcode & def.mMask) == def.mOpcodePattern)
        {
            return true;
        }
        return false;
    }

    void ApplyDefinition(const InstructionDef& def)
    {
        mOpcodePatternId = def.mOpcodePatternId;
        mDebug.mMask = def.mMask;
        mDebug.mOpcodePattern = def.mOpcodePattern;

        mArguments.clear();
        for (const auto& argDef : def.mArgs)
        {
            uint16_t value = (mDebug.mOpcode & argDef.mMask) >> argDef.mShift;
            mArguments.push_back(value);
        }
    }

    OpcodePatternId GetOpcodePatternId() const { return mOpcodePatternId; }
    const std::vector<uint16_t>& GetArguments() const { return mArguments; }
    const DebugInfo& GetDebugInfo() const { return mDebug; }

private:
    OpcodePatternId mOpcodePatternId;
    std::vector<uint16_t> mArguments;    
    DebugInfo mDebug;
};

//--------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Instruction& instruction);