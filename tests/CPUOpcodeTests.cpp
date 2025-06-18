#include <gtest/gtest.h>
#define UNIT_TESTING
#include "Emulator.h"

//--------------------------------------------------------------------------------
class CPUOpcodeTest : public ::testing::Test 
{
protected:
    Emulator mEmulator;

    //CPU& cpu() { return mEmulator.mCPU; }
    //RAM& ram() { return mEmulator.mRAM; }
    //Display& display() { return mEmulator.mDisplay; }
};