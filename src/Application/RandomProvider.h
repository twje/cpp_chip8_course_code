#pragma once

// Includes
//--------------------------------------------------------------------------------
// Project
#include "Interfaces/IRandomProvider.h"

//--------------------------------------------------------------------------------
class RandomProvider : public IRandomProvider
{
public:
	virtual uint8_t GetRandomByte() override;
};