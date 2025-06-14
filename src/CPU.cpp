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
    const uint16_t address = mPC;
    const uint16_t opcode = Fetch(address);

    // Advance PC early (some instructions override it)
    mPC += 2;

    // Ignore 0nnn (RCA 1802 call) — obsolete on modern interpreters
    if ((opcode & 0xF000) == 0x0000)
    {
        return;
    }

    // Decode
    std::optional<Instruction> instructionOpt = mDisassembler.TryGetInstruction(opcode, address);
    if (!instructionOpt.has_value())
    {
        throw std::runtime_error("Unknown or invalid opcode: " + std::to_string(opcode));
    }

    Execute(*instructionOpt);
}

//--------------------------------------------------------------------------------
uint16_t CPU::Fetch(uint16_t address)
{
    assert(address % 2 == 0);
    const uint8_t hByte = mBus.Read8(address);
    const uint8_t lByte = mBus.Read8(address + 1);

    // CHIP-8 opcodes are stored big-endian: high byte first in memory.
    return (static_cast<uint16_t>(hByte) << 8) | lByte;
}

//--------------------------------------------------------------------------------
void CPU::Execute(const Instruction& instruction)
{
    std::cout << instruction << std::endl;

    //std::cout << std::hex << instruction.mAddress << ": " << OpCodeIdToString(instruction.mOpcodeId) << std::endl;
}