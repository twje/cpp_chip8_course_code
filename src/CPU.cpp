#include "CPU.h"

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Instruction.h"

// System
#include <optional>
#include <iostream>
#include <cassert>

//--------------------------------------------------------------------------------
CPU::CPU(Bus& bus)
	: mBus{ bus }
    , mPC(PROGRAM_START_ADDRESS)
{ }

//--------------------------------------------------------------------------------
void CPU::Step()
{
    uint16_t opcode = Fetch();

    // Advance PC early (some instructions override it)
    mPC += 2;

    // Ignore 0nnn (RCA 1802 call) — obsolete on modern interpreters
    if ((opcode & 0xF000) == 0x0000)
    {
        return;
    }

    // Decode
    std::optional<Instruction> instructionOpt = mDisassembler.TryGetInstruction(opcode);
    if (!instructionOpt.has_value())
    {
        throw std::runtime_error("Unknown or invalid opcode: " + std::to_string(opcode));
    }

    Execute(*instructionOpt);
}

//--------------------------------------------------------------------------------
uint16_t CPU::Fetch()
{
    assert(mPC % 2 == 0);
    const uint8_t hByte = mBus.Read8(mPC);
    const uint8_t lByte = mBus.Read8(mPC + 1);

    // CHIP-8 opcodes are big-endian: high byte comes first. Convert 
    // to host-endian (little-endian on x86) for execution.
    return (static_cast<uint16_t>(hByte) << 8) | lByte;
}

//--------------------------------------------------------------------------------
void CPU::Execute(const Instruction& instruction)
{
    
}