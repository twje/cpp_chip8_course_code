#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Constants.h"
#include "UI/Layout/FramedWidgetBase.h"
#include "Platform/Olc/OlcVec.h"
#include "Interpreter/Bus.h"

// Third Party
#include "olcPixelGameEngine.h"

// System
#include <cassert>

//--------------------------------------------------------------------------------
class DisplayWidget : public FramedWidgetBase
{
public:
	DisplayWidget(olc::PixelGameEngine& pge)
		: FramedWidgetBase(pge, "Display")
		, mFramebuffer(DISPLAY_WIDTH, DISPLAY_HEIGHT)
	{
		mFrame.SetContentSize(GetInternalContentSize());
	}

	virtual void Draw(const ViewModel& viewModel) override
	{		
		UpdateFramebuffer(viewModel);
		DrawFramebuffer();
	}

private:
	void UpdateFramebuffer(const ViewModel& viewModel)
	{
		const Display& display = viewModel.mBus->mDisplay;

		for (int32_t y = 0; y < DISPLAY_HEIGHT; ++y)
		{
			for (int32_t x = 0; x < DISPLAY_WIDTH; ++x)
			{
				const olc::Pixel color = display.IsPixelSet(x, y)
					? UITheme::kColorScreenOn
					: UITheme::kColorScreenOff;

				mFramebuffer.SetPixel(x, y, color);
			}
		}		
	}

	void DrawFramebuffer()
	{
		mFrame.Draw(mPge);

		const olc::vi2d position = ToOLCVecInt(mFrame.GetContentOffset());
		const int32_t scale = UITheme::kPixelScale;		

		mPge.DrawSprite(position, &mFramebuffer, scale);
	}

	IntVec2 GetInternalContentSize() const
	{
		const olc::vi2d baseSize{ DISPLAY_WIDTH, DISPLAY_HEIGHT };
		return ToIntVec2(baseSize * UITheme::kPixelScale);
	}
		
	olc::Sprite mFramebuffer;	
};