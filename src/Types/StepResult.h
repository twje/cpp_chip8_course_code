#pragma once

// Includes
//--------------------------------------------------------------------------------
// Project
#include "ExecutionStatus.h"

//--------------------------------------------------------------------------------
struct StepResult
{
	ExecutionStatus mStatus = ExecutionStatus::Executed; // Default to executed	
	bool mShouldHalt = false;
};