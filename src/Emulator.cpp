#include "Emulator.h"

//--------------------------------------------------------------------------------
Emulator::Emulator()
	: mBus({ mRAM, mDisplay, mKeypad, mDelayTimer, mSoundTimer })
	, mCPU(mBus)
{ 
	// Add RAM
}

//--------------------------------------------------------------------------------
void Emulator::Run()
{
	mCPU.Step();
}