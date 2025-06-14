#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Instruction.h"

// System
#include <optional>

//--------------------------------------------------------------------------------
class Disassembler
{
public:
	std::optional<Instruction> TryGetInstruction(uint16_t opcode, uint16_t address);
};