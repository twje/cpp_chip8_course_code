#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Constants.h"

// Third Party
#include <olcPixelGameEngine.h>

// System
#include <string>
#include <string_view>

//--------------------------------------------------------------------------------
olc::vi2d GetMonospaceStringBlockSize(std::string_view sampleText, int32_t numLines);
std::string ToHexString(uint32_t value, uint8_t width);
std::string PadLeft(std::string_view text, size_t width);