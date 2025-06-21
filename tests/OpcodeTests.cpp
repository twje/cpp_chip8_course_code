#include <gtest/gtest.h>

#include "Emulator.h"
#include "Bus.h"
#include "RAM.h"

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, Opcode_3xkk_SE_VX_KK)
{
	Emulator emulator;
	RAM& ram = emulator.GetBus().mRAM;
}