#pragma once

//--------------------------------------------------------------------------------
enum class ExecutionState
{
	kWaitingForRom, // No ROM or waiting for ROM
	kStepping,		// Paused, manual stepping allowed
	kPlaying,		// Running automatically
	kHalted,		// Emulator error / stopped
	kNone
};