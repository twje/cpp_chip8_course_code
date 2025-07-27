#pragma once

// Includes
//--------------------------------------------------------------------------------
// Project
#include "ExecutionStatus.h"

// System
#include <cstdint>

//--------------------------------------------------------------------------------
struct FetchResult
{
	ExecutionStatus mStatus = ExecutionStatus::Executed; // Default to executed	
	uint16_t mOpcode = 0;
	bool mIsValidAddress = true;
};