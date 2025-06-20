#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "UIStyle.h"

// Third Party
#include "olcPixelGameEngine.h"

//--------------------------------------------------------------------------------
class WidgetFrame
{
	inline static const olc::vi2d kBorder{ UIStyle::kBorderSize, UIStyle::kBorderSize };
	inline static const olc::vi2d kPadding{ UIStyle::kPaddingSize, UIStyle::kPaddingSize };
	inline static constexpr int32_t kTextHeight = 8;
	inline static constexpr int32_t kGap = 1;

public:
	WidgetFrame(std::string title = { }, olc::vi2d contentSize = { })
		: mTitle(std::move(title))
		, mContentSize(contentSize)
	{ }

	void SetPosition(const olc::vi2d& position)
	{
		mPosition = position;
	}

	void SetContentSize(const olc::vi2d& size)
	{
		mContentSize = size;
	}

	void SetTitle(std::string title)
	{
		mTitle = std::move(title);
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
		return mPosition + kBorder + kPadding + GetTitleOffset();
	}

	void Draw(olc::PixelGameEngine& pge) const
	{
		// Background
		pge.FillRect(mPosition, GetSize(), UIStyle::kColorBG);

		// Title
		if (!mTitle.empty())
		{
			const olc::vi2d titlePos = mPosition + kBorder + kPadding;
			pge.DrawString(titlePos, mTitle, UIStyle::kColorAccent);

			const int32_t underlineY = titlePos.y + kTextHeight + kGap;

			// DrawLine is inclusive — subtract 1 to avoid overshooting the width
			pge.DrawLine(
				{ mPosition.x, underlineY },
				{ mPosition.x + GetSize().x - 1, underlineY },
				UIStyle::kColorBorder
			);
		}

		// Border
		pge.DrawRect(mPosition, GetSize() - olc::vi2d{ 1, 1 }, UIStyle::kColorBorder);
	}

private:
	olc::vi2d GetTitleSize() const
	{
		return mTitle.empty() ? olc::vi2d{ 0, 0 } : olc::vi2d{ 0, kTextHeight + kGap * 2 };
	}

	olc::vi2d GetTitleOffset() const
	{
		return mTitle.empty() ? olc::vi2d{ 0, 0 } : olc::vi2d{ 0, kTextHeight + kGap * 2 };
	}

private:
	std::string mTitle;
	olc::vi2d mPosition;
	olc::vi2d mContentSize;
};
