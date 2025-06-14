#include <iostream>

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "Bus.h  "

// System
#include <array>
#include <memory>

//--------------------------------------------------------------------------------
class CPU
{
public:
	CPU(Bus& bus)
		: mBus{ bus }
	{ }

	void Step(uint16_t opcode)
	{

	}

private:
	Bus& mBus;
};