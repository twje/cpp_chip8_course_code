#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "RomManager.h"
#include "Emulator.h"

// Third Party
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

//--------------------------------------------------------------------------------
class Application : public olc::PixelGameEngine
{
public:
	Application()
	{
		sAppName = "Chip8 Emulator";
	}

	bool OnUserCreate() override
	{
		RomManager romManager(ROMS_PATH);
	
		Emulator emulator;
		if (emulator.LoadRom(romManager.ResolveRom("test_rom/test_opcode.8o")))
		{
			emulator.Run();
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame
		for (int x = 0; x < ScreenWidth(); x++)
		{
			for (int y = 0; y < ScreenHeight(); y++)
			{
				Draw(x, y, olc::Pixel(rand() % 255, rand() % 255, rand() % 255));
			}
		}
			
		return true;
	}
};