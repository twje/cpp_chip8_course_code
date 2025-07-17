#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interfaces/IWidget.h"

// Third Party
#include "olcPixelGameEngine.h"

//--------------------------------------------------------------------------------
inline IntVec2 ToIntVec2(const olc::vi2d& vec)
{
	return { vec.x, vec.y };
}

//--------------------------------------------------------------------------------
inline olc::vi2d ToOLCVecInt(const IntVec2& vec)
{
	return { vec.mX, vec.mY };
}