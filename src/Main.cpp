#include <iostream>

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "Emulator.h"

//--------------------------------------------------------------------------------
int main()
{
	Emulator emulator;
	if (emulator.LoadRom("test_rom/test_opcode.8o"))
	{
		emulator.Run();
	}

	return 0;
}