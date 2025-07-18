#pragma once

// Includes
//--------------------------------------------------------------------------------
// System
#include <cstdint>

//--------------------------------------------------------------------------------
class IRandomProvider
{
public:
    virtual ~IRandomProvider() = default;
    virtual uint8_t GetRandomByte() = 0;
};