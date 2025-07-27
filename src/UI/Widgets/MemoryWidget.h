#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "UI/Layout/FramedWidgetBase.h"
#include "Platform/Olc/OlcVec.h"
#include "Constants.h"
#include "Utils/TextUtils.h"
#include "Interpreter/Bus.h"
#include "Interpreter/Instruction/Instruction.h"

// System
#include <cstdint>
#include <string>

//--------------------------------------------------------------------------------
class MemoryWidget : public FramedWidgetBase
{
    static constexpr int16_t kNumLines = 7;

public:
    explicit MemoryWidget(olc::PixelGameEngine& pge)
        : FramedWidgetBase(pge, "Memory")	
    {
		mFrame.SetContentSize(GetInternalContentSize());
    }

	virtual void Draw(const ViewModel& viewModel) override
    {
        const RAM& ram = viewModel.mBus->mRAM;
		const Snapshot& snapshot = viewModel.mSnapshot;

        mFrame.Draw(mPge);

        constexpr int16_t lineHeight = UI_CHAR_SIZE;        
        constexpr int16_t bytesPerLine = 2;
        constexpr int16_t halfLines = kNumLines / 2;

        const olc::vi2d start = ToOLCVecInt(mFrame.GetContentOffset());
        const uint16_t cursorAddress = snapshot.mAddress;

        // Compute display start address and cursor line position
        uint16_t startAddress = 0;
        int16_t cursorLine = 0;

        if (cursorAddress < halfLines * bytesPerLine)
        {
            cursorLine = static_cast<int16_t>(cursorAddress / bytesPerLine);
        }
        else
        {
            startAddress = static_cast<uint16_t>(cursorAddress - halfLines * bytesPerLine);
            cursorLine = halfLines;
        }

        const olc::Pixel textColor = viewModel.mIsDisplayInteractive ? olc::WHITE : olc::Pixel(192, 192, 192);

        for (int16_t i = 0; i < kNumLines; ++i)
        {
            uint16_t address = static_cast<uint16_t>(startAddress + i * bytesPerLine);
            if (address >= RAM_SIZE) { continue; }

            std::string prefix = (i == cursorLine) ? ">0x" : " 0x";
            std::string line = prefix + ToHexString(address, 4) + ": ";

            for (int16_t b = 0; b < bytesPerLine; ++b)
            {
                uint16_t byteAddress = static_cast<uint16_t>(address + b);
                line += (byteAddress < RAM_SIZE)
                    ? ToHexString(ram.Read(byteAddress), 2) + " "
                    : "?? ";
            }

            const olc::vi2d pos = start + olc::vi2d{ 0, i * lineHeight };
            mPge.DrawString(pos, line, textColor);
        }
    }

private:
    IntVec2 GetInternalContentSize() const
    {
        return ToIntVec2(GetMonospaceStringBlockSize("0x>FFFF: FF FF", 7));
    }  
};