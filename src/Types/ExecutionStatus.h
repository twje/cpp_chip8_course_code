#pragma once

//--------------------------------------------------------------------------------
enum class ExecutionStatus
{
	Executed,
	DecodeError,
	NotImplemented,
	MissingHandler,
	InvalidAddressUnaligned,
	InvalidAddressOutOfBounds
};