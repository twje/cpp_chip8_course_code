#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Platform/Olc/OlcVec.h"
#include "UI/Layout/FramedWidgetBase.h"

// System
#include <functional>
#include <optional>
#include <string>
#include <vector>

//--------------------------------------------------------------------------------
class RomListWidget : public FramedWidgetBase
{
    static constexpr float kScrollDelayTime = 0.25f;
    static constexpr float kScrollRepeatTime = 0.08f;
    static constexpr int32_t kScrollbarWidth = 6;
    static constexpr int32_t kArrowHeight = 8;

    inline static const olc::Pixel kScrollbarColor{ olc::GREY };
    inline static const olc::Pixel kScrollbarThumbColor{ olc::WHITE };
    inline static const olc::Pixel kArrowColor{ olc::VERY_DARK_GREY };

public:
    RomListWidget(olc::PixelGameEngine& pge)
        : FramedWidgetBase(pge, "Roms")        
        , mStartIndex(0)
        , mVisibleRows(4)
        , mScrollHoldTimer(0.0f)
        , mScrollRepeatTimer(0.0f)
        , mIsScrollingUpHeld(false)
        , mIsScrollingDownHeld(false)
    {
        mFrame.SetContentSize({ 0, static_cast<int32_t>(mVisibleRows) * UI_CHAR_SIZE });
    }

    void SetWidth(int32_t width)
    {
        mFrame.SetOuterSize({ width, mFrame.GetOuterSize().mY });
    }

    void SetRomList(const std::vector<std::string>& roms)
    {
        mRomList = roms;
        mStartIndex = 0;
        mSelectedIndex.reset();
    }

	void SelectRomAtIndex(size_t index)
	{
		if (index < mRomList.size())
		{
			mSelectedIndex = index;
			mStartIndex = index < mVisibleRows 
                ? 0 
                : index - mVisibleRows + 1; // Adjust start index to keep selected item visible
		    
            if (mOnRomSelectedCallback)
            {
				mOnRomSelectedCallback(index);
            }
        }
		else
		{
			mSelectedIndex.reset();
		}
	}

    void SetOnRomSelectedCallback(const std::function<void(size_t)>& callback)
    {
        mOnRomSelectedCallback = callback;
    }

    virtual void Update(float elapsedTime) override
    {
        HandleMouseInput();
        HandleScrollHolding(elapsedTime);
    }

    virtual void Draw(const ViewModel& viewModel) override
    {        
        (void)viewModel; // Unused parameter

        mFrame.Draw(mPge);
        DrawRomRows();
        DrawScrollbar();
    }

private:
    void DrawRomRows()
    {
        const IntVec2 basePos = mFrame.GetContentOffset();
        for (size_t row = 0; row < mVisibleRows; ++row)
        {
            size_t romIndex = mStartIndex + row;
            if (romIndex >= mRomList.size())
            {
                break;
            }

            const IntVec2 rowPos = { basePos.mX, basePos.mY + static_cast<int32_t>(row * UI_CHAR_SIZE) };
            const std::string& label = mRomList[romIndex];

            if (mSelectedIndex && *mSelectedIndex == romIndex)
            {
                mPge.FillRect(ToOLCVecInt(rowPos), { mFrame.GetContentSize().mX, UI_CHAR_SIZE }, olc::DARK_CYAN);
            }

            mPge.DrawString(ToOLCVecInt(rowPos), label, UITheme::kColorText);
        }
    }

    void DrawScrollbar()
    {
        const IntVec2 basePos = mFrame.GetContentOffset();
        const IntVec2 contentSize = mFrame.GetContentSize();

        const IntVec2 scrollbarPos = { basePos.mX + contentSize.mX - kScrollbarWidth, basePos.mY };
        const int32_t totalHeight = contentSize.mY;

        mPge.FillRect(ToOLCVecInt(scrollbarPos), { kScrollbarWidth, totalHeight }, kScrollbarColor);

        mPge.FillTriangle(
            { scrollbarPos.mX + kScrollbarWidth / 2, scrollbarPos.mY + 2 },
            { scrollbarPos.mX + 2, scrollbarPos.mY + kArrowHeight - 2 },
            { scrollbarPos.mX + kScrollbarWidth - 2, scrollbarPos.mY + kArrowHeight - 2 },
            kArrowColor);

        mPge.FillTriangle(
            { scrollbarPos.mX + 2, scrollbarPos.mY + totalHeight - kArrowHeight + 2 },
            { scrollbarPos.mX + kScrollbarWidth - 2, scrollbarPos.mY + totalHeight - kArrowHeight + 2 },
            { scrollbarPos.mX + kScrollbarWidth / 2, scrollbarPos.mY + totalHeight - 2 },
            kArrowColor);

        if (mRomList.size() > mVisibleRows)
        {
            int32_t scrollableArea = totalHeight - 2 * kArrowHeight;
            int32_t thumbHeight = std::max(1, scrollableArea * static_cast<int32_t>(mVisibleRows) / static_cast<int32_t>(mRomList.size()));
            int32_t thumbOffset = scrollableArea * static_cast<int32_t>(mStartIndex) / static_cast<int32_t>(mRomList.size());

            IntVec2 thumbPos = { scrollbarPos.mX, scrollbarPos.mY + kArrowHeight + thumbOffset };
            mPge.FillRect(ToOLCVecInt(thumbPos), { kScrollbarWidth, thumbHeight }, kScrollbarThumbColor);
        }
    }

    void HandleMouseInput()
    {
        const IntVec2 mousePos = { mPge.GetMouseX(), mPge.GetMouseY() };
        const IntVec2 basePos = mFrame.GetContentOffset();
        const IntVec2 contentSize = mFrame.GetContentSize();
        const IntVec2 scrollbarPos = { basePos.mX + contentSize.mX - kScrollbarWidth, basePos.mY };
        const int32_t totalHeight = contentSize.mY;

        bool mouseHeld = mPge.GetMouse(0).bHeld;
        bool mouseClicked = mPge.GetMouse(0).bPressed;

        IntVec2 upArrowArea = { scrollbarPos.mX, scrollbarPos.mY };
        IntVec2 downArrowArea = { scrollbarPos.mX, scrollbarPos.mY + totalHeight - kArrowHeight };

        if (mouseClicked)
        {
            if (IsMouseInRect(mousePos, upArrowArea, { kScrollbarWidth, kArrowHeight }, false))
            {
                ScrollUp();
                mIsScrollingUpHeld = true;
                mScrollHoldTimer = 0.0f;
            }
            else if (IsMouseInRect(mousePos, downArrowArea, { kScrollbarWidth, kArrowHeight }, false))
            {
                ScrollDown();
                mIsScrollingDownHeld = true;
                mScrollHoldTimer = 0.0f;
            }
        }
        else if (!mouseHeld)
        {
            mIsScrollingUpHeld = false;
            mIsScrollingDownHeld = false;
            mScrollHoldTimer = 0.0f;
        }

        for (size_t row = 0; row < mVisibleRows; ++row)
        {
            IntVec2 rowPos = { basePos.mX, basePos.mY + static_cast<int32_t>(row * UI_CHAR_SIZE) };
            if (IsMouseInRect(mousePos, rowPos, { contentSize.mX, UI_CHAR_SIZE }, true) && mouseClicked)
            {
                size_t romIndex = mStartIndex + row;
                if (romIndex < mRomList.size())
                {
                    mSelectedIndex = romIndex;
                    if (mOnRomSelectedCallback)
                    {
                        mOnRomSelectedCallback(romIndex);
                    }
                }
            }
        }

        int32_t scrollDelta = mPge.GetMouseWheel();
        if (scrollDelta > 0)
        {
            ScrollUp();
        }
        else if (scrollDelta < 0)
        {
            ScrollDown();
        }
    }

    void HandleScrollHolding(float elapsedTime)
    {
        if (mIsScrollingUpHeld || mIsScrollingDownHeld)
        {
            mScrollHoldTimer += elapsedTime;

            if (mScrollHoldTimer > kScrollDelayTime)
            {
                mScrollRepeatTimer += elapsedTime;
                if (mScrollRepeatTimer >= kScrollRepeatTime)
                {
                    if (mIsScrollingUpHeld) { ScrollUp(); }
                    if (mIsScrollingDownHeld) { ScrollDown(); }
                    mScrollRepeatTimer = 0.0f;
                }
            }
        }
    }

    bool IsMouseInRect(const IntVec2& mousePos, const IntVec2& rectPos, const IntVec2& rectSize, bool excludeScrollbar) const
    {
        int32_t effectiveWidth = rectSize.mX;
        if (excludeScrollbar)
        {
            effectiveWidth -= kScrollbarWidth;
        }

        const int32_t endX = rectPos.mX + effectiveWidth;
        const int32_t endY = rectPos.mY + rectSize.mY;

		const bool inRangeX = (mousePos.mX >= rectPos.mX) && (mousePos.mX < endX);
		const bool inRangeY = (mousePos.mY >= rectPos.mY) && (mousePos.mY < endY);

		return inRangeX && inRangeY;
    }

    void ScrollUp()
    {
        if (mStartIndex > 0)
        {
            mStartIndex--;
        }
    }

    void ScrollDown()
    {
        if (mStartIndex + mVisibleRows < mRomList.size())
        {
            mStartIndex++;
        }
    }

    std::vector<std::string> mRomList;
    std::optional<size_t> mSelectedIndex;
    size_t mStartIndex;
    size_t mVisibleRows;

    float mScrollHoldTimer;
    float mScrollRepeatTimer;
    bool mIsScrollingUpHeld;
    bool mIsScrollingDownHeld;

    std::function<void(size_t)> mOnRomSelectedCallback;
};
