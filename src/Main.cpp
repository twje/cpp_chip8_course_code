#include <iostream>

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "RomManager.h"
#include "Emulator.h"

//--------------------------------------------------------------------------------
int main()
{
	RomManager romManager(ROMS_PATH);
	
	Emulator emulator;
	if (emulator.LoadRom(romManager.ResolveRom("test_rom/test_opcode.8o")))
	{
		emulator.Run();
	}

	return 0;
}