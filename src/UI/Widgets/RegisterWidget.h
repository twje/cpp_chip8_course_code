#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "UI/Layout/FramedWidgetBase.h"
#include "Platform/Olc/OlcVec.h"
#include "Platform/Olc/OlcVec.h"
#include "Constants.h"
#include "Utils/TextUtils.h"
#include "Interpreter/Hardware/CPU.h"

// System
#include <optional>
#include <cstdint>
#include <string>
#include <cassert>

//--------------------------------------------------------------------------------
class RegisterWidget : public FramedWidgetBase
{
public:
	explicit RegisterWidget(olc::PixelGameEngine& pge)
		: FramedWidgetBase(pge, "Registers")	
	{
		mFrame.SetContentSize(ToIntVec2(GetInternalContentSize()));
	}

	void Draw(const ViewModel& viewModel) override
	{
		const CPUState& state = viewModel.mSnapshot.mCPUState;

		mFrame.Draw(mPge);		

		const olc::vi2d offset = ToOLCVecInt(mFrame.GetContentOffset());
		const olc::Pixel textColor = viewModel.mIsDisplayInteractive ? olc::WHITE : olc::Pixel(192, 192, 192);
		const int lineHeight = UI_CHAR_SIZE;

		// Draw general-purpose registers V0-VF
		for (uint8_t i = 0; i < REGISTER_COUNT; ++i)
		{
			const olc::vi2d pos = offset + olc::vi2d{ 0, i * lineHeight };
			const std::string text = "V" + ToHexString(i, 1) + ": 0x" + ToHexString(state.mRegisters[i], 2);
			mPge.DrawString(pos, text, textColor);
		}

		// Draw specific registers below general-purpose ones
		auto drawField = [&](int line, const std::string& label, const std::string& value) {
			const olc::vi2d pos = offset + olc::vi2d{ 0, line * lineHeight };
			mPge.DrawString(pos, PadLeft(label, 3) + " " + value, textColor);
		};

		const int baseLine = REGISTER_COUNT + 1;
		drawField(baseLine + 0, "PC:", "0x" + ToHexString(state.mProgramCounter, 4));
		drawField(baseLine + 1, "I:", "0x" + ToHexString(state.mIndexRegister, 4));
		drawField(baseLine + 2, "SP:", "0x" + ToHexString(state.mStackPointer, 1));
		drawField(baseLine + 3, "DT:", "0x" + ToHexString(state.mDelayTimer, 2));
		drawField(baseLine + 4, "ST:", "0x" + ToHexString(state.mSoundTimer, 2));
	}

private:
	olc::vi2d GetInternalContentSize() const
	{
		const char* sample = "PC: 0x0200";  // Longest label + value string
		return GetMonospaceStringBlockSize(sample, REGISTER_COUNT + 6);
	}
};