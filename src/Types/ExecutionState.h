#pragma once

//--------------------------------------------------------------------------------
enum class ExecutionState
{
	WAITING_FOR_ROM, // No ROM or waiting for ROM
	STEPPING,		// Paused, manual stepping allowed
	PLAYING,		// Running automatically
	HALTED,		// Emulator error / stopped
	NONE
};