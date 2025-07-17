#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Constants.h"

// System
#include <array>

//--------------------------------------------------------------------------------
struct CPUState
{
	// Core CPU State
	std::array<uint8_t, REGISTER_COUNT> mRegisters{ }; // V0-VF
	uint16_t mIndexRegister = 0;  // I
	uint16_t mProgramCounter = 0; // PC
	uint16_t mCycleCount = 0;

	// Stack
	uint8_t mStackPointer = 0; // SP
	std::array<uint16_t, STACK_SIZE> mStack{ };

	// Timers
	uint8_t mDelayTimer = 0;
	uint8_t mSoundTimer = 0;
};
