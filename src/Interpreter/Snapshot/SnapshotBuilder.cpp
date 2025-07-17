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
	const FetchResult fetch = cpu.Fetch();
    
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

    const std::vector<uint16_t>& operands = mInstruction.GetOperands();
    const OpcodeSpec& spec = OpcodeTable::Get(opcodeId);
    assert(operands.size() == spec.mOperands.size());

    std::vector<OperandInfo> result;
    result.reserve(operands.size());

    for (size_t i = 0; i < operands.size(); ++i)
    {
        result.push_back({ spec.mOperands[i].mLabel, operands[i] });
    }

    return result;
}