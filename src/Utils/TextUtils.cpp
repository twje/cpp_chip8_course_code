#include "Utils/TextUtils.h"

//--------------------------------------------------------------------------------
olc::vi2d GetMonospaceStringBlockSize(std::string_view sampleText, int32_t numLines)
{
	return {
		static_cast<int32_t>(sampleText.size()) * UI_CHAR_SIZE,
		numLines * UI_CHAR_SIZE
	};
}

//--------------------------------------------------------------------------------
std::string ToHexString(uint32_t value, uint8_t width)
{
	std::string s(width, '0');
	for (int i = width - 1; i >= 0; --i, value >>= 4)
	{
		s[i] = "0123456789ABCDEF"[value & 0xF];
	}
	return s;
}

//--------------------------------------------------------------------------------
std::string PadLeft(std::string_view label, size_t width)
{
	if (label.size() >= width)
	{
		return std::string(label);
	}
	return std::string(width - label.size(), ' ') + std::string(label);
}