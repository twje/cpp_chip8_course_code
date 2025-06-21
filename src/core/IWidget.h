#pragma once

// Includes
//--------------------------------------------------------------------------------
// Third Party
#include "olcPixelGameEngine.h"

//--------------------------------------------------------------------------------
class IWidget
{
public:
	virtual ~IWidget() = default;

	virtual olc::vi2d GetSize() const = 0;
	virtual olc::vi2d GetPosition() const = 0;
	virtual void SetPosition(const olc::vi2d& position) = 0;
	virtual void Draw(olc::PixelGameEngine& pge) const = 0;
};
