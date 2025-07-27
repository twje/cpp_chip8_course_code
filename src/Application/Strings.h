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
		static inline constexpr const char* kRunning = "Running";
	};

	//--------------------------------------------------------------------------------
	inline std::string ExecutionStatusToString(ExecutionStatus status)
	{
		switch (status)
		{
			case ExecutionStatus::EXECUTED: return "Opcode executed";
			case ExecutionStatus::DECODE_ERROR: return "Opcode decode error";
			case ExecutionStatus::NOT_IMPLEMENTED: return "Opcode not implemented";
			case ExecutionStatus::MISSING_HANDLER: return "Opcode handler missing";
			case ExecutionStatus::INVALID_ADDRESS_UNALIGNED: return "Address is unaligned";
			case ExecutionStatus::INVALID_ADDRESS_OUT_OF_BOUNDS: return "Address out of bounds";
			default: return "Unknown execution status";
		}
	}

	//--------------------------------------------------------------------------------
	inline const char* ExecutionStateToString(ExecutionState state)
	{
		switch (state)
		{
			case ExecutionState::WAITING_FOR_ROM: return "WaitingForRom";
			case ExecutionState::STEPPING: return "Stepping";
			case ExecutionState::PLAYING: return "Playing";
			case ExecutionState::HALTED: return "Halted";
			case ExecutionState::NONE: return "None";
			default: return "Unknown";
		}
	}
}
