#pragma once

// Includes
//--------------------------------------------------------------------------------
#include "Platform/Olc/OlcVec.h"
#include "UI/Layout/FramedWidgetBase.h"
#include "Platform/Olc/OlcVec.h"
#include "Interfaces/IWidget.h"

// System
#include <string>

//--------------------------------------------------------------------------------
class NotificationWidget : public FramedWidgetBase
{
	static constexpr int32_t kPadding = 1;

public:
	NotificationWidget(olc::PixelGameEngine& pge)
		: FramedWidgetBase(pge)		
	{
		// Height is fixed; width is set by UIManager layout
		mFrame.SetContentSize({ 0, UI_CHAR_SIZE + kPadding * 2 });
	}

	void SetWidth(int32_t width)
	{
		mFrame.SetOuterSize({ width, mFrame.GetOuterSize().mY });
	}

	void SetText(const std::string& text)
	{
		mText = text;
	}

	virtual void Draw(const ViewModel& viewModel) override
	{		
		(void)viewModel; // Unused parameter

		mFrame.Draw(mPge);

		const olc::vi2d position = ToOLCVecInt(mFrame.GetContentOffset()) + olc::vi2d{ kPadding, kPadding };
		mPge.DrawString(position, mText, UITheme::kColorText);
	}

private:
	std::string mText;
};