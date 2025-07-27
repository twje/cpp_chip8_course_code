#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Constants.h"
#include "Utils/TextUtils.h"
#include "Platform/Olc/OlcVec.h"
#include "UI/Layout/FramedWidgetBase.h"

// System
#include <string>

//--------------------------------------------------------------------------------
class InstructionWidget : public FramedWidgetBase
{
public:
	explicit InstructionWidget(olc::PixelGameEngine& pge)		
		: FramedWidgetBase(pge, "Instruction")				
	{
		mFrame.SetContentSize(ToIntVec2(GetInternalContentSize()));
	}

	void Draw(const ViewModel& viewModel) override
	{
		const Snapshot& snapshot = viewModel.mSnapshot;

		mFrame.Draw(mPge);		

		const olc::vi2d offset = ToOLCVecInt(mFrame.GetContentOffset());
                constexpr int labelWidth = 10;
		const olc::Pixel textColor = viewModel.mIsDisplayInteractive ? olc::WHITE : olc::Pixel(192, 192, 192);  // White / Light Gray
                constexpr int lineHeight = UI_CHAR_SIZE;

		auto drawField = [&](int line, const std::string& label, const std::string& value) {
			const olc::vi2d pos = offset + olc::vi2d{ 0, line * lineHeight };
			mPge.DrawString(pos, PadLeft(label, labelWidth) + " " + value, textColor);
		};

		drawField(0, "Cycle:", std::to_string(snapshot.mCycleCount));
		drawField(1, "Address:", "0x" + ToHexString(snapshot.mAddress, 4));
		drawField(2, "Opcode:", "0x" + ToHexString(snapshot.mOpcode, 4));
		drawField(3, "Pattern:", snapshot.mPattern);
		drawField(4, "Mnemonic:", snapshot.mMnemonic);

		drawField(5, "Operand 1:", FormatOperand(snapshot, 0));
		drawField(6, "Operand 2:", FormatOperand(snapshot, 1));
	}

private:
	olc::vi2d GetInternalContentSize() const
	{
		return GetMonospaceStringBlockSize(" Mnemonic: DRW_VX_VY_N", 7);
	}

	std::string FormatOperand(const Snapshot& snapshot, size_t index) const
	{
		if (index >= snapshot.mOperands.size())
		{
			return "-";
		}

		const auto& op = snapshot.mOperands[index];
		return "0x" + ToHexString(op.mValue, 3) + " (" + op.mLabel + ")";
	}
};