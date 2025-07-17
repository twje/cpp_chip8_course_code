#pragma once

//  Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interfaces/IKeyInputProvider.h"

// Third Party
#include "olcPixelGameEngine.h"

//--------------------------------------------------------------------------------
class OlcKeyInputProvider : public IKeyInputProvider
{
public:
	explicit OlcKeyInputProvider(const olc::PixelGameEngine& pge)
		: mPge(pge)
	{ }

	bool IsKeyPressed(uint8_t key) const override
	{
		olc::Key olcKey = static_cast<olc::Key>(key);
		olc::HWButton state = mPge.GetKey(olcKey);
		return state.bHeld || state.bPressed;
	}

private:
	const olc::PixelGameEngine& mPge;
};
