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
	inline static constexpr int32_t kCharWidth = 8;

public:
	WidgetFrame() = default;

	explicit WidgetFrame(const std::string& title)
		: mTitle(title)
	{ }

	void SetPosition(const olc::vi2d& position) { mPosition = position; }
	void SetContentSize(const olc::vi2d& size) { mContentSize = size; }

	olc::vi2d GetSize() const
	{
		return kBorder * 2 + kPadding * 2 + olc::vi2d{ 0, GetTitleBlockHeight() } + mContentSize;
	}

	olc::vi2d GetPosition() const { return mPosition; }

	olc::vi2d GetContentOffset() const
	{
		return mPosition + kBorder + kPadding + olc::vi2d{ 0, GetTitleBlockHeight() };
	}

	void Draw(olc::PixelGameEngine& pge) const
	{
		pge.FillRect(mPosition, GetSize(), UIStyle::kColorBG);

		if (!mTitle.empty())
		{
			const olc::vi2d titlePos = GetCenteredTitlePosition();
			pge.DrawString(titlePos, mTitle, UIStyle::kColorAccent);

			const int32_t underlineY = titlePos.y + kTextHeight + kGap;
			pge.DrawLine(
				{ mPosition.x, underlineY },
				{ mPosition.x + GetSize().x - 1, underlineY },
				UIStyle::kColorBorder
			);
		}
		
		pge.DrawRect(mPosition, GetSize() - olc::vi2d{ 1, 1 }, UIStyle::kColorBorder);
	}

private:
	int32_t GetTitleBlockHeight() const
	{
		// Text + gap + underline + gap to content
		return mTitle.empty() ? 0 : kTextHeight + kGap + kLineHeight + kGap;
	}

	olc::vi2d GetCenteredTitlePosition() const
	{
		const int32_t titleWidth = static_cast<int32_t>(mTitle.length()) * kCharWidth;
		const int32_t contentWidth = GetSize().x - (kBorder.x + kPadding.x) * 2;
		const int32_t centeredX = mPosition.x + kBorder.x + kPadding.x + (contentWidth - titleWidth) / 2;
		const int32_t y = mPosition.y + kBorder.y + kPadding.y;
		return { centeredX, y };
	}

private:
	std::string mTitle;
	olc::vi2d mPosition;
	olc::vi2d mContentSize;
};
