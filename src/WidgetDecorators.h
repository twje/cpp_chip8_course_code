#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "IWidget.h"
#include "UIStyle.h"

// Third Party
#include "olcPixelGameEngine.h"

// System
#include <memory>

// Fills the widget's background with a solid color
//--------------------------------------------------------------------------------
class BackgroundDecorator : public IWidget
{
public:
	BackgroundDecorator(std::unique_ptr<IWidget> inner)
		: mInner(std::move(inner))
	{ }

	virtual olc::vi2d GetSize() const override { return mInner->GetSize(); }
	virtual olc::vi2d GetPosition() const override { return mPosition; }

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mPosition = position;
		mInner->SetPosition(position);
	}

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		pge.FillRect(mPosition, GetSize(), UIStyle::kColorBG);
		mInner->Draw(pge);
	}

private:
	std::unique_ptr<IWidget> mInner;	
	olc::vi2d mPosition;
};

// Adds a text title above the widget with an underline
//--------------------------------------------------------------------------------
class TitleDecorator : public IWidget
{
	static constexpr int32_t kTextHeight = 8;
	static constexpr int32_t kGapBelowText = 1;
	static constexpr int32_t kUnderlineHeight = 1;
	static constexpr int32_t kGapBelowLine = 1;
	static constexpr int32_t kTotalTitleHeight = kTextHeight + kGapBelowText + kUnderlineHeight + kGapBelowLine;

public:
	TitleDecorator(std::unique_ptr<IWidget> inner, std::string title)
		: mInner(std::move(inner)), mTitle(std::move(title))
	{
		SetPosition(mInner->GetPosition());
	}

	virtual olc::vi2d GetSize() const override
	{
		return mInner->GetSize() + olc::vi2d{ 0, kTotalTitleHeight };
	}

	virtual olc::vi2d GetPosition() const override
	{
		return mPosition;
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mPosition = position;
		mInner->SetPosition(position + olc::vi2d{ 0, kTotalTitleHeight });
	}

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		pge.DrawString(mPosition, mTitle, UIStyle::kColorAccent);

		const int32_t underlineY = mPosition.y + kTextHeight + kGapBelowText;

		// DrawLine is inclusive on both ends — subtract 1 to avoid overshooting the intended width
		pge.DrawLine(
			{ mPosition.x, underlineY },
			{ mPosition.x + GetSize().x - 1, underlineY },
			UIStyle::kColorBorder
		);

		mInner->Draw(pge);
	}

private:
	std::unique_ptr<IWidget> mInner;
	std::string mTitle;
	olc::vi2d mPosition;
};

//--------------------------------------------------------------------------------
class BorderDecorator : public IWidget
{
	inline static const olc::vi2d kBorderPadding { UIStyle::kBorderSize, UIStyle::kBorderSize };

public:
	BorderDecorator(std::unique_ptr<IWidget> inner)
		: mInner(std::move(inner))
	{ 
		SetPosition(mInner->GetPosition());
	}

	virtual olc::vi2d GetSize() const override
	{		
		return mInner->GetSize() + kBorderPadding * 2;
	}
	
	virtual olc::vi2d GetPosition() const override { return mPosition; }

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mPosition = position;

		// Offset the inner widget to accomodate border
		olc::vi2d innerPos = position + kBorderPadding;
		mInner->SetPosition(innerPos);
	}

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		// DrawRect is inclusive — subtract {1,1} to avoid overshooting the intended size
		pge.DrawRect(mPosition, GetSize() - olc::vi2d{ 1, 1 }, UIStyle::kColorBorder);
		mInner->Draw(pge);		
	}

private:
	std::unique_ptr<IWidget> mInner;
	olc::vi2d mPosition;
};