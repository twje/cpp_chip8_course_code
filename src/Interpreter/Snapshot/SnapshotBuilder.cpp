#include "Interpreter/Snapshot/SnapshotBuilder.h"

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interpreter/Hardware/CPU.h"
#include "Interpreter/Instruction/OpcodeTable.h"

// System
#include <vector>
#include <cassert>
#include <string>

//--------------------------------------------------------------------------------
SnapshotBuilder::SnapshotBuilder(const CPU& cpu, uint64_t cycleCount)
    : mCPU(cpu)
    , mCycleCount(cycleCount)
{
	FetchResult fetch = cpu.Peek();
    
    mSnapshot.mCPUState = cpu.GetState();
    mSnapshot.mAddress = mSnapshot.mCPUState.mProgramCounter;
    mSnapshot.mOpcode = fetch.mOpcode;

	// Only try to decode if address is valid
    if (fetch.mIsValidAddress)
    {
        mInstruction = cpu.Decode(fetch.mOpcode);
    }

    mSnapshot.mDecodeSucceeded = mInstruction.IsValid();
}

//--------------------------------------------------------------------------------
Snapshot SnapshotBuilder::Build()
{
    if (!mSnapshot.mDecodeSucceeded)
    {
        return mSnapshot;
    }

    mSnapshot.mCycleCount = mCycleCount;

    const OpcodeId opcodeId = mInstruction.GetOpcodeId();
    const OpcodeSpec& spec = OpcodeTable::Get(opcodeId);

    mSnapshot.mPattern = spec.mPatternStr;
    mSnapshot.mMnemonic = spec.mMnemonic;
    mSnapshot.mOperands = ToOperandInfoList();

    return mSnapshot;
}

//--------------------------------------------------------------------------------
std::vector<OperandInfo> SnapshotBuilder::ToOperandInfoList()
{
    OpcodeId opcodeId = mInstruction.GetOpcodeId();
    assert(opcodeId != OpcodeId::UNASSIGNED);

    const OpcodeSpec& spec = OpcodeTable::Get(opcodeId);
    if (spec.mOperands.empty())
    {
        // No operands for this opcode
		return { };
    }

    std::vector<OperandInfo> result;
    result.reserve(spec.mOperands.size());

    for (const auto& operandSpec : spec.mOperands)
    {
        OperandType kind = operandSpec.mKind;
        uint16_t value = 0;

        switch (kind)
        {
            case OperandType::NNN: value = mInstruction.GetOperandNNN(); break;
            case OperandType::KK:  value = mInstruction.GetOperandKK();  break;
            case OperandType::N:   value = mInstruction.GetOperandN();   break;
            case OperandType::X:   value = static_cast<uint16_t>(mInstruction.GetOperandX()); break;
            case OperandType::Y:   value = static_cast<uint16_t>(mInstruction.GetOperandY()); break;
        }

        result.push_back({ operandSpec.mLabel, value });
    }

    return result;
}