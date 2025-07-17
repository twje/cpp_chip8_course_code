#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Types/ExecutionState.h"
#include "Types/ExecutionStatus.h"

namespace Strings
{
	//--------------------------------------------------------------------------------
	struct Notifications
	{
		static inline constexpr const char* kPleaseSelectRom = "Please select a ROM";
		static inline constexpr const char* kWaitingForStepInput = "Waiting for Step Input";
		static inline constexpr const char* kRunning = "Running...";
	};

	//--------------------------------------------------------------------------------
	inline std::string ExecutionStatusToString(ExecutionStatus status)
	{
		switch (status)
		{
			case ExecutionStatus::Executed: return "Opcode Executed";
			case ExecutionStatus::DecodeError: return "Opcode Decode Error";
			case ExecutionStatus::NotImplemented: return "Opcode Not Implemented";
			case ExecutionStatus::MissingHandler: return "Opcode Handler Missing";
			default: return "Unknown";
		}
	}

	//--------------------------------------------------------------------------------
	inline const char* ExecutionStateToString(ExecutionState state)
	{
		switch (state)
		{
			case ExecutionState::kWaitingForRom: return "WaitingForRom";
			case ExecutionState::kStepping: return "Stepping";
			case ExecutionState::kPlaying: return "Playing";
			case ExecutionState::kHalted: return "Halted";
			case ExecutionState::kNone: return "None";
			default: return "Unknown";
		}
	}
}
