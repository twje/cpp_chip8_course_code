#include <iostream>

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "RomManager.h"
#include "Emulator.h"

// Third Party
#define OLC_PGE_APPLICATION  // Compile PGE implementation here
#include "olcPixelGameEngine.h"
  
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