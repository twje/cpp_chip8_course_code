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
			case ExecutionStatus::Executed: return "Opcode executed";
			case ExecutionStatus::DecodeError: return "Opcode decode error";
			case ExecutionStatus::NotImplemented: return "Opcode not implemented";
			case ExecutionStatus::MissingHandler: return "Opcode handler missing";
			case ExecutionStatus::InvalidAddressUnaligned: return "Address is unaligned";
			case ExecutionStatus::InvalidAddressOutOfBounds: return "Address out of bounds";
			default: return "Unknown execution status";
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
