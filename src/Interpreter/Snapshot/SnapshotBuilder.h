#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interpreter/Instruction/Instruction.h"
#include "Interpreter/Snapshot/Snapshot.h"

// Forward Declarations
//--------------------------------------------------------------------------------
class CPU;

//--------------------------------------------------------------------------------
class SnapshotBuilder
{
public:
    SnapshotBuilder(const CPU& cpu, uint64_t cycleCount);

    Snapshot Build();

private:
    std::vector<OperandInfo> ToOperandInfoList();

    const CPU& mCPU;
    uint64_t mCycleCount;
    Instruction mInstruction;
    Snapshot mSnapshot;
};