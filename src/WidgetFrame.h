#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "UIStyle.h"

// Third Party
#include "olcPixelGameEngine.h"

/*
	Adds a title, padding, and border for a widget.
	- Call SetContentSize() with the inner content's size.
	- Use GetContentOffset() as the top-left for drawing content.
	- GetSize() returns the full frame size including decorations.
*/
//--------------------------------------------------------------------------------
class WidgetFrame
{
	inline static const olc::vi2d kBorder{ UIStyle::kBorderSize, UIStyle::kBorderSize };
	inline static const olc::vi2d kPadding{ UIStyle::kPaddingSize, UIStyle::kPaddingSize };
	inline static constexpr int32_t kTextHeight = 8;
	inline static constexpr int32_t kLineHeight = 1;
	inline static constexpr int32_t kGap = 1;

public:
	WidgetFrame() = default;

	WidgetFrame(const std::string& title)
		: mTitle(title)
	{ }

	void SetPosition(const olc::vi2d& position)
	{
		mPosition = position;
	}

	void SetContentSize(const olc::vi2d& size)
	{
		mContentSize = size;
	}

	olc::vi2d GetSize() const
	{
		return kBorder * 2 + kPadding * 2 + GetTitleSize() + mContentSize;
	}

	olc::vi2d GetPosition() const
	{
		return mPosition;
	}

	olc::vi2d GetContentOffset() const
	{
		return mPosition + kBorder + kPadding + GetTitleSize();
	}

	void Draw(olc::PixelGameEngine& pge) const
	{
		pge.FillRect(mPosition, GetSize(), UIStyle::kColorBG);

		if (!mTitle.empty())
		{
			const olc::vi2d titlePos = mPosition + kBorder + kPadding;
			pge.DrawString(titlePos, mTitle, UIStyle::kColorAccent);

			// One pixel gap after title, then line
			const int32_t underlineY = titlePos.y + kTextHeight + kGap;

			// DrawLine is inclusive — subtract 1 to avoid overshooting the width
			pge.DrawLine(
				{ mPosition.x, underlineY },
				{ mPosition.x + GetSize().x - 1, underlineY },
				UIStyle::kColorBorder
			);
		}

		pge.DrawRect(mPosition, GetSize() - olc::vi2d{ 1, 1 }, UIStyle::kColorBorder);
	}

private:
	olc::vi2d GetTitleSize() const
	{
		// Title + 1px gap + 1px underline + 1px gap
		return mTitle.empty()
			? olc::vi2d{ 0, 0 }
		: olc::vi2d{ 0, kTextHeight + kGap * 2 + kLineHeight };
	}

private:
	std::string mTitle;
	olc::vi2d mPosition;
	olc::vi2d mContentSize;
};