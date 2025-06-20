#pragma once

// Includes
//--------------------------------------------------------------------------------
// Third Party
#include "olcPixelGameEngine.h"

// Centralized UI style constants for layout, colors, and spacing
//--------------------------------------------------------------------------------
struct UIStyle
{
	// Spacing and sizing
	static constexpr int32_t kBorderSize = 1;
	static constexpr int32_t kPaddingSize = 1;
	static constexpr int32_t kWidgetSpacing = 4;

	// Colors
	inline static const olc::Pixel kColorText = olc::WHITE;					// USED
	inline static const olc::Pixel kColorAccent = olc::YELLOW;				// USED
	inline static const olc::Pixel kColorActive = olc::RED;					// USED
	inline static const olc::Pixel kColorInactive = olc::DARK_GREY;
	inline static const olc::Pixel kColorBorder = olc::GREY;				// USED
	inline static const olc::Pixel kColorBG = olc::VERY_DARK_GREY;			// Used
	inline static const olc::Pixel kColorScreenOn = olc::GREEN;				// USED
	inline static const olc::Pixel kColorScreenOff = olc::VERY_DARK_GREY;	// USED
};