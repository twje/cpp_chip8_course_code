#pragma once

// Inputs
//--------------------------------------------------------------------------------
// Interpreter
#include "Constants.h"
#include "UI/UITheme.h"
#include "Platform/Olc/OlcVec.h"
#include "Interfaces/IWidget.h"

// Third Party
#include "olcPixelGameEngine.h"

//--------------------------------------------------------------------------------
class WidgetFrame
{
	inline static const olc::vi2d kBorder{ UITheme::kBorderSize, UITheme::kBorderSize };
	inline static const olc::vi2d kPadding{ UITheme::kPaddingSize, UITheme::kPaddingSize };
	inline static constexpr int32_t kLineHeight = 1;
	inline static constexpr int32_t kGap = 1;

public:
	WidgetFrame() = default;

	explicit WidgetFrame(const std::string& title)
		: mTitle(title)
	{ }

	void SetPosition(const IntVec2& position)
	{
		mPosition = { position.mX, position.mY };
	}

	void SetOuterSize(const IntVec2& size)
	{
		const olc::vi2d totalBorderPadding = 2 * (kBorder + kPadding);
		const olc::vi2d titleHeightOffset = { 0, GetTitleBlockHeight() };

		mContentSize = olc::vi2d{ size.mX, size.mY } - totalBorderPadding - titleHeightOffset;
	}

	void SetContentSize(const IntVec2& size)
	{
		mContentSize = { size.mX, size.mY };
	}

	IntVec2 GetPosition() const
	{
		return { mPosition.x, mPosition.y };
	}

	IntVec2 GetOuterSize() const
	{
		const auto size = CalculateOuterSize();
		return { size.x, size.y };
	}

	IntVec2 GetContentSize() const
	{
		return { mContentSize.x, mContentSize.y };
	}

	IntVec2 GetContentOffset() const
	{
		const auto offset = mPosition + kBorder + kPadding + olc::vi2d{ 0, GetTitleBlockHeight() };
		return { offset.x, offset.y };
	}

	void Draw(olc::PixelGameEngine& pge) const
	{
		const auto outerSize = CalculateOuterSize();

		pge.FillRect(mPosition, outerSize, UITheme::kColorBG);

		if (!mTitle.empty())
		{
			const olc::vi2d titlePos = GetCenteredTitlePosition(outerSize);
			pge.DrawString(titlePos, mTitle, UITheme::kColorAccent);

			const int32_t underlineY = titlePos.y + UI_CHAR_SIZE + kGap;
			pge.DrawLine(
				{ mPosition.x, underlineY },
				{ mPosition.x + outerSize.x - 1, underlineY },
				UITheme::kColorBorder
			);
		}

		pge.DrawRect(mPosition, outerSize - olc::vi2d{ 1, 1 }, UITheme::kColorBorder);
	}

private:
	int32_t GetTitleBlockHeight() const
	{
		return mTitle.empty() ? 0 : UI_CHAR_SIZE + kGap + kLineHeight + kGap;
	}

	olc::vi2d GetCenteredTitlePosition(const olc::vi2d& outerSize) const
	{
		const int32_t titleWidth = static_cast<int32_t>(mTitle.length()) * UI_CHAR_SIZE;
		const int32_t contentWidth = outerSize.x - 2 * (kBorder.x + kPadding.x);
		const int32_t centeredX = mPosition.x + kBorder.x + kPadding.x + (contentWidth - titleWidth) / 2;
		const int32_t y = mPosition.y + kBorder.y + kPadding.y;
		return { centeredX, y };
	}

	olc::vi2d CalculateOuterSize() const
	{
		return 2 * kBorder + 2 * kPadding + olc::vi2d{ 0, GetTitleBlockHeight() } + mContentSize;
	}

	std::string mTitle;
	olc::vi2d mPosition;
	olc::vi2d mContentSize;
};

//--------------------------------------------------------------------------------
class FramedWidgetBase : public IWidget
{
public:
	FramedWidgetBase(olc::PixelGameEngine& pge, const std::string& title = "")
		: mPge(pge)
		, mFrame(title)
	{ }

	virtual IntVec2 GetSize() const override final
	{ 
		return mFrame.GetOuterSize(); 		
	}
	
	virtual IntVec2 GetPosition() const override final
	{ 
		return mFrame.GetPosition();
	}

	virtual void SetPosition(const IntVec2& position) override final
	{ 
		mFrame.SetPosition(position);
	}

	virtual void Update(float) override { }
	virtual void Draw(const ViewModel&) override { }

protected:
	olc::PixelGameEngine& mPge;
	WidgetFrame mFrame;
};
