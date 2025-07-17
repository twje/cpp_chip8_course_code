#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "UI/Layout/FramedWidgetBase.h"
#include "Platform/Olc/OlcVec.h"
#include "Constants.h"
#include "Utils/TextUtils.h"
#include "Interpreter/Hardware/CPU.h"

// System
#include <cstdint>
#include <string>

//--------------------------------------------------------------------------------
class StackWidget : public FramedWidgetBase
{
public:
	StackWidget(olc::PixelGameEngine& pge)
		: FramedWidgetBase(pge, "Stack")		
	{
		mFrame.SetContentSize(GetInternalContentSize());
	}

	virtual void Draw(const ViewModel& viewModel) override
	{
		const CPUState& state = viewModel.mSnapshot.mCPUState;

		mFrame.Draw(mPge);		
		
		const olc::vi2d offset = ToOLCVecInt(mFrame.GetContentOffset());
		const int32_t lineHeight = UI_CHAR_SIZE;

		for (uint8_t i = 0; i < STACK_SIZE; ++i)
		{
			const bool isTop = (i == state.mStackPointer);
			const std::string cursor = isTop ? ">" : " ";

			const olc::vi2d pos = offset + olc::vi2d{ 0, static_cast<int32_t>(i) * lineHeight };
			const std::string text = cursor + ToHexString(i, 1) + ": 0x" + ToHexString(state.mStack[i], 4);

			const olc::Pixel textColor = viewModel.mIsDisplayInteractive ? olc::WHITE : olc::Pixel(192, 192, 192);  // White / Light Gray
			mPge.DrawString(pos, text, textColor);
		}
	}

private:
	IntVec2 GetInternalContentSize() const
	{
		return ToIntVec2(GetMonospaceStringBlockSize(" F: 0xFFFF ", STACK_SIZE));
	}	
};