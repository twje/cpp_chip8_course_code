#include <iostream>

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "Bus.h"

// System
#include <array>

//--------------------------------------------------------------------------------
class CPU
{
public:
	CPU(Bus& bus);

	void Step(uint16_t opcode);

private:
	Bus& mBus;
};