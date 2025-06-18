#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "RomManager.h"
#include "Emulator.h"
#include "ExecutionStatus.h"

// Third Party
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
		if (!mEmulator.LoadRom(romManager.ResolveRom("test_rom/test_opcode.8o")))
		{
			std::cerr << "Unable to load ROM" << std::endl;
			return false;
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		CPU& cpu = mEmulator.GetCPU();		
		
		Instruction instruction = cpu.Fetch();
		if (cpu.Decode(instruction))
		{
			cpu.Execute(instruction);
		}


		//while (true)
		
		/*StepResult result = mEmulator.Step();

		switch (result.mStatus)
		{

		}*/
		

		//if (GetKey(olc::Key::SPACE).bPressed)
		//{
		//	mEmulator.Step();
		//}

		//Clear(olc::DARK_BLUE);
		//DrawLine(10, 10, ScreenWidth() - 10, 10, olc::YELLOW);			

		return true;
	}

private:
	Emulator mEmulator;
};