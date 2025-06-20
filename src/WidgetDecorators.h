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

//--------------------------------------------------------------------------------
class TitleDecorator : public IWidget
{
	static constexpr int32_t kTitleHeight = 9;

public:
	TitleDecorator(std::unique_ptr<IWidget> inner, std::string title)
		: mInner(std::move(inner)), mTitle(std::move(title))
	{
		SetPosition(mInner->GetPosition());
	}

	virtual olc::vi2d GetSize() const override
	{
		// Title sits above the inner widget
		return mInner->GetSize() + olc::vi2d{ 0, kTitleHeight };
	}

	virtual olc::vi2d GetPosition() const override
	{
		return mPosition;
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mPosition = position;

		// Offset the inner widget below the title
		olc::vi2d innerPos = position + olc::vi2d{ 0, kTitleHeight };
		mInner->SetPosition(innerPos);
	}

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		// Draw title text
		pge.DrawString(mPosition, mTitle, UIStyle::kColorAccent);

		// Draw underline (DrawLine is inclusive, so subtract 1 from width)
		const olc::vi2d lineStart = mPosition + olc::vi2d{ 0, kTitleHeight - 1 };
		const olc::vi2d lineEnd = lineStart + olc::vi2d{ GetSize().x - 1, 0 };
		pge.DrawLine(lineStart, lineEnd, UIStyle::kColorBorder);
		
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

		pge.DrawRect({0, 0}, {1, 1}, UIStyle::kColorBorder);
	}

private:
	std::unique_ptr<IWidget> mInner;
	olc::vi2d mPosition;
};