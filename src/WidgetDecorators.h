#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "IWidget.h"

// Third Party
#include "olcPixelGameEngine.h"

//--------------------------------------------------------------------------------
class BackgroundDecorator : public IWidget
{
public:
	BackgroundDecorator(std::unique_ptr<IWidget> inner, olc::Pixel color)
		: mInner(std::move(inner))
		, mColor(color)
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
		pge.FillRect(mPosition, GetSize(), mColor);
		mInner->Draw(pge);
	}

private:
	std::unique_ptr<IWidget> mInner;
	olc::Pixel mColor;
	olc::vi2d mPosition;
};

//--------------------------------------------------------------------------------
class TitleDecorator : public IWidget
{
	static constexpr int32_t TitleHeight = 9;

public:
	TitleDecorator(std::unique_ptr<IWidget> inner, std::string title)
		: mInner(std::move(inner)), mTitle(std::move(title))
	{
		SetPosition(mInner->GetPosition());
	}

	virtual olc::vi2d GetSize() const override
	{
		// Title sits above the inner widget
		return mInner->GetSize() + olc::vi2d{ 0, TitleHeight };
	}

	virtual olc::vi2d GetPosition() const override
	{
		return mPosition;
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mPosition = position;

		// Offset the inner widget below the title
		olc::vi2d innerPos = position + olc::vi2d{ 0, TitleHeight };
		mInner->SetPosition(innerPos);
	}

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		// Draw the title string
		pge.DrawString(mPosition, mTitle, olc::YELLOW);

		// Draw a horizontal separator line beneath the title
		olc::vi2d lineStart = mPosition + olc::vi2d{ 0, TitleHeight - 1 };
		olc::vi2d lineEnd = olc::vi2d{ GetSize().x - 1, TitleHeight - 1 };
		pge.DrawLine(lineStart, lineEnd, olc::DARK_GREY);

		mInner->Draw(pge);
	}

private:
	std::unique_ptr<IWidget> mInner;
	std::string mTitle;
	olc::vi2d mPosition;
};