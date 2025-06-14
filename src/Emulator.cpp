#include "Emulator.h"

//--------------------------------------------------------------------------------
Emulator::Emulator()
	: mBus({ mRAM, mDisplay, mKeypad, mDelayTimer, mSoundTimer })
	, mCPU(mBus)
{ }

//--------------------------------------------------------------------------------
void Emulator::Run()
{

}