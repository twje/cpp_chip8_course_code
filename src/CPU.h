#include <iostream>

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Bus.h"
#include "Disassembler.h"

// System
#include <array>

//--------------------------------------------------------------------------------
class CPU
{
public:
	CPU(Bus& bus);

	void Step();

private:
	uint16_t Fetch();
	void Execute(const Instruction& instruction);
	
	Bus& mBus;
	Disassembler mDisassembler;
	
	uint16_t mPC;
};