#pragma once

//--------------------------------------------------------------------------------
enum class ExecutionStatus
{
    Executed,
    Ignored,
    DecodeError,
    NotImplemented,
    MissingHandler
};