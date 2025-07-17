#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Constants.h"

// Third Party
#include <olcPixelGameEngine.h>

// System
#include <string>

//--------------------------------------------------------------------------------
olc::vi2d GetMonospaceStringBlockSize(const std::string& sampleText, int32_t numLines);
std::string ToHexString(uint32_t value, uint8_t width);
std::string PadLeft(const std::string& text, size_t width);