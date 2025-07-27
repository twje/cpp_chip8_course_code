#pragma once

//--------------------------------------------------------------------------------
enum class ExecutionStatus
{
	EXECUTED,
	DECODE_ERROR,
	NOT_IMPLEMENTED,
	MISSING_HANDLER,
	WAITING_ON_KEY_PRESS,
	INVALID_ADDRESS_UNALIGNED,
	INVALID_ADDRESS_OUT_OF_BOUNDS
};