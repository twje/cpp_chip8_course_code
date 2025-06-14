#include "Emulator.h"

// Includes
//--------------------------------------------------------------------------------
// System
#include <fstream>

//--------------------------------------------------------------------------------
Emulator::Emulator()
	: mBus({ mRAM, mDisplay, mKeypad, mDelayTimer, mSoundTimer })
	, mCPU(mBus)
{ 
	// Add RAM
}

//--------------------------------------------------------------------------------
bool Emulator::LoadRom(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);

	if (!file)
	{		
		std::cerr << "Could not open file : " + filename << std::endl;
		return false;
	}

	// Seek to the end to determine the file size
	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// TODO: implement

}

//--------------------------------------------------------------------------------
void Emulator::Run()
{
	mCPU.Step();
}