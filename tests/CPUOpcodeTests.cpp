#include <gtest/gtest.h>
#define UNIT_TESTING
#include "Emulator.h"

//--------------------------------------------------------------------------------
class TestEmulator 
{
public:    
    CPU& cpu() { return mEmulator.mCPU; }
    RAM& ram() { return mEmulator.mRAM; }
    Display& display() { return mEmulator.mDisplay; }

private:
    Emulator mEmulator;
};
