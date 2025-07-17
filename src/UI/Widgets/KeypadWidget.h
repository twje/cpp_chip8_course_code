#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Constants.h"
#include "UI/Layout/FramedWidgetBase.h"
#include "Interpreter/Bus.h"

// System
#include <cstdint>
#include <string>
#include <string_view>

//--------------------------------------------------------------------------------
class KeypadWidget : public FramedWidgetBase
{
	// Layout constants	
	static constexpr int32_t kKeySpacing = 5;
	static constexpr int32_t kRows = 4;
	static constexpr int32_t kCols = 4;

	// Visual layout of CHIP-8 keys in 4x4 grid (directly using Keypad::Key)
	inline static constexpr Keypad::Key KeyLayout[kRows][kCols] = {
		{ Keypad::Key::Key1, Keypad::Key::Key2, Keypad::Key::Key3, Keypad::Key::KeyC },
		{ Keypad::Key::Key4, Keypad::Key::Key5, Keypad::Key::Key6, Keypad::Key::KeyD },
		{ Keypad::Key::Key7, Keypad::Key::Key8, Keypad::Key::Key9, Keypad::Key::KeyE },
		{ Keypad::Key::KeyA, Keypad::Key::Key0, Keypad::Key::KeyB, Keypad::Key::KeyF }
	};

public:
	KeypadWidget(olc::PixelGameEngine& pge)		
		: FramedWidgetBase(pge, "Keypad")		
	{
		mFrame.SetContentSize(GetInternalContentSize());
	}

	virtual void Draw(const ViewModel& viewModel) override
	{
		Keypad& keypad = viewModel.mBus->mKeypad;

		mFrame.Draw(mPge);

		const olc::vi2d start = ToOLCVecInt(mFrame.GetContentOffset());

		for (int32_t y = 0; y < kRows; ++y)
		{
			for (int32_t x = 0; x < kCols; ++x)
			{
				olc::vi2d offset = {
					x * (UI_CHAR_SIZE + kKeySpacing),
					y * (UI_CHAR_SIZE + kKeySpacing)
				};

				Keypad::Key key = KeyLayout[y][x];
				std::string_view label = Keypad::GetLabelForKey(key);

				const bool isPressed = keypad.IsKeyPressed(key);
				const olc::Pixel color = isPressed ? UITheme::kColorHighlightText : UITheme::kColorText;

				mPge.DrawString(start + offset, label.data(), color);
			}
		}
	}

private:
	IntVec2 GetInternalContentSize() const
	{
		int32_t totalWidth = kCols * UI_CHAR_SIZE + (kCols - 1) * kKeySpacing;
		int32_t totalHeight = kRows * UI_CHAR_SIZE + (kRows - 1) * kKeySpacing;

		return { totalWidth, totalHeight };
	}	
};