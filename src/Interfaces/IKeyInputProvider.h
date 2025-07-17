#pragma once

//--------------------------------------------------------------------------------
// System
#include <cstdint>

//--------------------------------------------------------------------------------
class IKeyInputProvider
{
public:
    virtual ~IKeyInputProvider() = default;
    virtual bool IsKeyPressed(uint8_t physicalKey) const = 0;
};