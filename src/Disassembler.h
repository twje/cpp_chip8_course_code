#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Instruction.h"

// Forward Declaration
//--------------------------------------------------------------------------------
struct Instruction;

//--------------------------------------------------------------------------------
class Disassembler
{
public:
	bool Decode(Instruction& outInstruction);
};