#pragma once

// Includes
//------------------------------------------------------------------------------
// Interpreter
#include "Interpreter/Instruction/OpcodeId.h"

// System
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

//------------------------------------------------------------------------------
enum class OperandType
{
    NNN,
    KK,
    N,
    X,
    Y
};

//------------------------------------------------------------------------------
struct OperandSpec
{
    uint16_t mMask = 0;
    uint8_t mShift = 0;
    OperandType mKind = OperandType::NNN;
    const char* mLabel = nullptr; // e.g. "x", "nnn"
};

//------------------------------------------------------------------------------
struct OpcodeSpec
{
	OpcodeId mOpcodeId = OpcodeId::UNASSIGNED;
    uint16_t mMask = 0;
    uint16_t mPattern = 0;
    const char* mPatternStr = nullptr; // e.g. "8xy1"
    const char* mMnemonic = nullptr;   // e.g. "OR"
    std::vector<OperandSpec> mOperands;
};

//------------------------------------------------------------------------------
class OpcodeTable
{
public:
    static const OpcodeSpec& Get(OpcodeId id);
    static const std::vector<OpcodeSpec>& All();

private:
    static const std::vector<OpcodeSpec> mTable;
};