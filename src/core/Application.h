#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "IWidget.h"
#include "WidgetFrame.h"
#include "WidgetDecorators.h"
#include "RomManager.h"
#include "Emulator.h"

// Temp
#include "Utils.h"

// Third Party
#include "olcPixelGameEngine.h"

// System
#include <string>

//--------------------------------------------------------------------------------
class DisplayUI : public IWidget
{
public:
	DisplayUI(const Display& display)
		: mDisplay(display)
		, mPixelScale(4)
		, mFrame("Display")
	{ 
		mFrame.SetContentSize(GetInternalContentSize());
	}

	virtual olc::vi2d GetSize() const override
	{
		return mFrame.GetSize();
	}

	virtual olc::vi2d GetPosition() const override
	{
		return mFrame.GetPosition();
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mFrame.SetPosition(position);
	}

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		mFrame.Draw(pge);

		const olc::vi2d pixelSize{ mPixelScale, mPixelScale };

		for (int y = 0; y < DISPLAY_HEIGHT; ++y)
		{
			for (int x = 0; x < DISPLAY_WIDTH; ++x)
			{
				olc::Pixel color = mDisplay.IsPixelSet(x, y)
					? UIStyle::kColorScreenOn
					: UIStyle::kColorScreenOff;
								
				olc::vi2d position = mFrame.GetContentOffset() +  olc::vi2d{ x, y } * mPixelScale;
				pge.FillRect(position, pixelSize, color);
			}
		}
	}

private:
	olc::vi2d GetInternalContentSize() const
	{
		olc::vi2d baseSize{ static_cast<int32_t>(DISPLAY_WIDTH), static_cast<int32_t>(DISPLAY_HEIGHT) };
		return baseSize * mPixelScale;
	}

	const Display& mDisplay;		
	WidgetFrame mFrame;
	int32_t mPixelScale;
};

//--------------------------------------------------------------------------------
class StackUI : public IWidget
{
public:
	StackUI(const CPU& cpu)
		: mCPU(cpu)
		, mFrame("Stack")
	{ 
		mFrame.SetContentSize(GetInternalContentSize());
	}

	virtual olc::vi2d GetSize() const override
	{
		return mFrame.GetSize();
	}

	virtual olc::vi2d GetPosition() const override
	{
		return mFrame.GetPosition();
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mFrame.SetPosition(position);
	}

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{		
		mFrame.Draw(pge);

		const CPUState& cpuState = mCPU.GetState();
		const olc::vi2d offset = mFrame.GetContentOffset();
		const int32_t lineHeight = 8;

		for (size_t i = 0; i < STACK_SIZE; ++i)
		{
			const bool isTop = (i == cpuState.mSP);
			const std::string cursor = isTop ? ">" : " ";

			const olc::vi2d pos = offset + olc::vi2d{ 0, static_cast<int32_t>(i) * lineHeight };
			const std::string text = cursor + Hex(i, 1) + ": 0x" + Hex(cpuState.mStack[i], 4);

			pge.DrawString(pos, text, UIStyle::kColorText);
		}
	}

private:
	olc::vi2d GetInternalContentSize() const
	{
		const std::string sampleLabel = " F: 0xFFFF"; // Longest possible line
		const int32_t charWidth = 8;
		const int32_t charHeight = 8;

		return {
			static_cast<int32_t>(sampleLabel.size()) * charWidth,
			STACK_SIZE * charHeight
		};
	}

	std::string Hex(uint32_t value, uint8_t width) const
	{
		std::string s(width, '0');
		for (int i = width - 1; i >= 0; --i, value >>= 4)
		{
			s[i] = "0123456789ABCDEF"[value & 0xF];
		}
		return s;
	}

	const CPU& mCPU;	
	WidgetFrame mFrame;
};

//--------------------------------------------------------------------------------
class RegisterUI : public IWidget
{
public:
	RegisterUI(const CPU& cpu)
		: mCPU(cpu)		
		, mFrame("Registers")
	{ 
		mFrame.SetContentSize(GetInternalContentSize());
	}

	virtual olc::vi2d GetSize() const override
	{
		return mFrame.GetSize();
	}

	virtual olc::vi2d GetPosition() const override
	{
		return mFrame.GetPosition();
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mFrame.SetPosition(position);
	}

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		mFrame.Draw(pge);

		const CPUState& cpuState = mCPU.GetState();
		const olc::vi2d offset = mFrame.GetContentOffset();
		const int32_t lineHeight = 8;

		for (size_t i = 0; i < REGISTER_COUNT; ++i)
		{
			olc::vi2d pos = offset + olc::vi2d{ 0, static_cast<int32_t>(i) * lineHeight };
			std::string text = Hex(i, 1) + ": 0x" + Hex(cpuState.mV[i], 2);

			pge.DrawString(pos, text, UIStyle::kColorText);
		}
	}

private:
	olc::vi2d GetInternalContentSize() const
	{
		const std::string sampleLabel = "F: 0xFF  "; // Longest possible line (whitespace to accomodate title)
		const int32_t charWidth = 8;
		const int32_t charHeight = 8;

		return {
			static_cast<int32_t>(sampleLabel.size()) * charWidth,
			STACK_SIZE * charHeight
		};
	}

	std::string Hex(uint32_t value, uint8_t width) const  // TODO: reused
	{
		std::string s(width, '0');
		for (int i = width - 1; i >= 0; --i, value >>= 4)
		{
			s[i] = "0123456789ABCDEF"[value & 0xF];
		}
		return s;
	}

	const CPU& mCPU;	
	WidgetFrame mFrame;
};

//--------------------------------------------------------------------------------
class CPUStateUI : public IWidget  // TODO: rename class to CPU
{
public:
	CPUStateUI()
		: mCycle(0)
		, mFrame("CPU")
	{ 
		mFrame.SetContentSize(GetInternalContentSize());
	}

	void UpdateDisplay(const CPUState& cpuState, size_t cycle)
	{
		mCPUState = cpuState;
		mCycle = cycle;
	}

	virtual olc::vi2d GetSize() const override  
	{ 
		return mFrame.GetSize();
	}
	
	virtual olc::vi2d GetPosition() const override
	{
		return mFrame.GetPosition();
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mFrame.SetPosition(position);
	}
	
	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		mFrame.Draw(pge);
		
		const olc::vi2d offset = mFrame.GetContentOffset();
		const int32_t lineHeight = 8;

		auto DrawLine = [&](int lineIndex, const std::string& text)
		{
			olc::vi2d pos = offset + olc::vi2d{ 0, lineIndex * lineHeight };
			pge.DrawString(pos, text, UIStyle::kColorText);
		};

		const int labelWidth = 8;
		
		DrawLine(0, RightAlign("Cycle:", labelWidth) + " " + std::to_string(mCycle));
		DrawLine(1, RightAlign("PC:", labelWidth) + " 0x" + Hex(mCPUState.mPC, 4));
		DrawLine(2, RightAlign("I:", labelWidth) + " 0x" + Hex(mCPUState.mI, 4));
		DrawLine(3, RightAlign("Delay:", labelWidth) + " 0x" + Hex(mCPUState.mDelayTimer, 2));
		DrawLine(4, RightAlign("Sound:", labelWidth) + " 0x" + Hex(mCPUState.mSoundTimer, 2));
	}

private:
	olc::vi2d GetInternalContentSize() const
	{
		std::string longestSample = "Pattern: UNKNOWN";
		                                      
		const int32_t lineHeight = 8;
		const int32_t lineCount = 5;
		const int32_t textWidth = longestSample.size() * 8;
		return { textWidth, lineCount * lineHeight };
	}

	std::string Hex(uint32_t value, uint8_t width) const
	{
		std::string s(width, '0');
		for (int i = width - 1; i >= 0; --i, value >>= 4)
		{
			s[i] = "0123456789ABCDEF"[value & 0xF];
		}
		return s;
	}

	std::string RightAlign(const std::string& label, size_t width) const
	{
		if (label.size() >= width) return label;
		return std::string(width - label.size(), ' ') + label;
	}

	size_t mCycle;
	CPUState mCPUState;
	olc::vi2d mPosition;
	WidgetFrame mFrame;
};

//--------------------------------------------------------------------------------
class InstructionUI : public IWidget
{
public:
	InstructionUI()
		: mFrame("Instruction")
	{
		mFrame.SetContentSize(GetInternalContentSize());
	}
	void UpdateDisplay(const InstructionInfo& info)
	{
		mInfo = info;
	}

	virtual olc::vi2d GetSize() const override
	{
		return mFrame.GetSize();
	}

	virtual olc::vi2d GetPosition() const override
	{
		return mFrame.GetPosition();
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mFrame.SetPosition(position);
	}

	void Draw(olc::PixelGameEngine& pge) const override
	{
		mFrame.Draw(pge);
		const olc::vi2d offset = mFrame.GetContentOffset();
		const int labelWidth = 10;

		DrawField(pge, offset, 0, labelWidth, "Address:", "0x" + Hex(mInfo.mAddress, 4));
		DrawField(pge, offset, 1, labelWidth, "Opcode:", "0x" + Hex(mInfo.mOpcode, 4));
		DrawField(pge, offset, 2, labelWidth, "Pattern:", mInfo.mPattern);
		DrawField(pge, offset, 3, labelWidth, "Mnemonic:", mInfo.mMnemonic);

		const OperandInfo* op1 = mInfo.mOperands.size() > 0 ? &mInfo.mOperands[0] : nullptr;
		const OperandInfo* op2 = mInfo.mOperands.size() > 1 ? &mInfo.mOperands[1] : nullptr;

		DrawField(pge, offset, 4, labelWidth, "Operand 1:", FormatOperand(op1));
		DrawField(pge, offset, 5, labelWidth, "Operand 2:", FormatOperand(op2));
	}

private:
	olc::vi2d GetInternalContentSize() const
	{
		std::string longestSample = " Mnemonic: DRW_VX_VY_N";  // Empty space is due to alignment
								    
		const int32_t lineHeight = 8;
		const int32_t lineCount = 6;
		const int32_t textWidth = longestSample.size() * 8;
		return { textWidth, lineCount * lineHeight };
	}

	std::string Hex(uint32_t value, uint8_t width) const
	{
		std::string s(width, '0');
		for (int i = width - 1; i >= 0; --i, value >>= 4)
		{
			s[i] = "0123456789ABCDEF"[value & 0xF];
		}
		return s;
	}

	std::string RightAlign(const std::string& label, size_t width) const
	{
		if (label.size() >= width) return label;
		return std::string(width - label.size(), ' ') + label;
	}
	
	std::string FormatOperand(const OperandInfo* op) const
	{
		if (!op) { return "-"; }
		return "0x" + Hex(op->mValue, 3) + " (" + op->mLabel + ")";
	}

	void DrawField(olc::PixelGameEngine& pge, const olc::vi2d& offset, int line, int labelWidth,
		const std::string& label, const std::string& value) const
	{
		const int lineHeight = 8;
		olc::vi2d pos = offset + olc::vi2d{ 0, line * lineHeight };
		pge.DrawString(pos, RightAlign(label, labelWidth) + " " + value, UIStyle::kColorText);
	}

	olc::vi2d mPosition;
	InstructionInfo mInfo;
	WidgetFrame mFrame;
};

//--------------------------------------------------------------------------------
enum class Chip8Key : uint8_t
{
	Key0, 
	Key1, 
	Key2, 
	Key3,
	Key4, 
	Key5, 
	Key6, 
	Key7,
	Key8, 
	Key9, 
	KeyA, 
	KeyB,
	KeyC, 
	KeyD, 
	KeyE, 
	KeyF
};

//--------------------------------------------------------------------------------
class KeypadUI : public IWidget
{
	// Layout constants
	static constexpr int kCharPixelSize = 8;
	static constexpr int kKeySpacing = 5;
	static constexpr int kRows = 4;
	static constexpr int kCols = 4;

	// Visual layout of CHIP-8 keys in 4x4 grid
	inline static constexpr Chip8Key KeyLayout[kRows][kCols] = {
		{ Chip8Key::Key1, Chip8Key::Key2, Chip8Key::Key3, Chip8Key::KeyC },
		{ Chip8Key::Key4, Chip8Key::Key5, Chip8Key::Key6, Chip8Key::KeyD },
		{ Chip8Key::Key7, Chip8Key::Key8, Chip8Key::Key9, Chip8Key::KeyE },
		{ Chip8Key::KeyA, Chip8Key::Key0, Chip8Key::KeyB, Chip8Key::KeyF }
	};

public:
	KeypadUI(const Keypad& keypad)
		: mKeypad(keypad)
		, mFrame("Keypad")
	{
		mFrame.SetContentSize(GetInternalContentSize());
	}

	virtual olc::vi2d GetSize() const override { return mFrame.GetSize(); }
	virtual olc::vi2d GetPosition() const override { return mFrame.GetPosition(); }
	virtual void SetPosition(const olc::vi2d& position) override { mFrame.SetPosition(position); }

public:	
	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		mFrame.Draw(pge);

		const olc::vi2d start = mFrame.GetContentOffset();

		for (int y = 0; y < kRows; ++y)
		{
			for (int x = 0; x < kCols; ++x)
			{
				olc::vi2d offset = {
					x * (kCharPixelSize + kKeySpacing),
					y * (kCharPixelSize + kKeySpacing)
				};

				Chip8Key key = KeyLayout[y][x];
				std::string_view label = GetLabelForKey(key);

				pge.DrawString(start + offset, label.data(), olc::WHITE);
			}
		}
	}

private:	
	olc::vi2d GetInternalContentSize() const
	{
		int32_t totalWidth = kCols * kCharPixelSize + (kCols - 1) * kKeySpacing;
		int32_t totalHeight = kRows * kCharPixelSize + (kRows - 1) * kKeySpacing;

		return { totalWidth, totalHeight };
	}

	std::string_view GetLabelForKey(Chip8Key key) const
	{
		switch (key)
		{
			case Chip8Key::Key0: return "0";
			case Chip8Key::Key1: return "1";
			case Chip8Key::Key2: return "2";
			case Chip8Key::Key3: return "3";
			case Chip8Key::Key4: return "4";
			case Chip8Key::Key5: return "5";
			case Chip8Key::Key6: return "6";
			case Chip8Key::Key7: return "7";
			case Chip8Key::Key8: return "8";
			case Chip8Key::Key9: return "9";
			case Chip8Key::KeyA: return "A";
			case Chip8Key::KeyB: return "B";
			case Chip8Key::KeyC: return "C";
			case Chip8Key::KeyD: return "D";
			case Chip8Key::KeyE: return "E";
			case Chip8Key::KeyF: return "F";
			default: return "?";
		}
	}

	const Keypad& mKeypad;
	WidgetFrame mFrame;
};

//--------------------------------------------------------------------------------
class StatusUI : public IWidget
{	
	static constexpr int32_t kCharPixelHeight = 8;
	static constexpr int32_t kPadding = 1;

public:
	StatusUI()
	{
		mFrame.SetContentSize({ 0, kCharPixelHeight + kPadding });
	}
	
	void SetWidth(int32_t width)
	{
		mFrame.SetContentSize({ width, kCharPixelHeight + kPadding });
	}

	void SetText(const std::string& text) { mText = text; }

	virtual olc::vi2d GetSize() const override { return mFrame.GetSize(); }
	virtual olc::vi2d GetPosition() const override { return mFrame.GetPosition(); }
	virtual void SetPosition(const olc::vi2d& position) override { mFrame.SetPosition(position); }

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		mFrame.Draw(pge);
		pge.DrawString(mFrame.GetContentOffset() + olc::vi2d{ kPadding, kPadding }, mText);
		// TODO: Add text color, UIStyle is the source of truth
	}

private:
	WidgetFrame mFrame;
	std::string mText;
};

//--------------------------------------------------------------------------------
class MemoryUI : public IWidget
{
public:
	MemoryUI(const RAM& ram)
		: mRAM(ram)
		, mCurrentAddress(0)
		, mFrame("Memory")
	{
		mFrame.SetContentSize(GetInternalContentSize());
	}

	void SetAddress(uint16_t address)
	{
		mCurrentAddress = address;
	}

	virtual olc::vi2d GetSize() const override { return mFrame.GetSize(); }
	virtual olc::vi2d GetPosition() const override { return mFrame.GetPosition(); }
	virtual void SetPosition(const olc::vi2d& position) override { mFrame.SetPosition(position); }

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		mFrame.Draw(pge);

		const int32_t lineHeight = 8;
		const int32_t numLines = 6;
		const int32_t bytesPerLine = 2;
		const int32_t halfLines = numLines / 2;
		const olc::vi2d start = mFrame.GetContentOffset();

		for (int i = 0; i < numLines; ++i)
		{
			int32_t offset = i - halfLines;
			uint16_t address = mCurrentAddress + offset * bytesPerLine;

			if (address >= RAM_SIZE)
			{
				continue;
			}

			std::string prefix = (address == mCurrentAddress) ? ">0x" : " 0x";
			std::string line = prefix + Hex(address, 4) + ": ";

			for (int b = 0; b < bytesPerLine; ++b)
			{
				uint16_t byteAddress = address + b;
				if (byteAddress < RAM_SIZE)
				{
					line += Hex(mRAM.Read(byteAddress), 2) + " ";
				}
				else
				{
					line += "?? ";
				}
			}

			olc::vi2d pos = start + olc::vi2d{ 0, i * lineHeight };
			pge.DrawString(pos, line, UIStyle::kColorText);
		}
	}

private:
	olc::vi2d GetInternalContentSize() const
	{
		const std::string sample = "0x>FFFF: FF FF";
		const int32_t charWidth = 8;
		const int32_t lineHeight = 8;
		const int32_t numLines = 6;

		return {
			static_cast<int32_t>(sample.size()) * charWidth,
			numLines * lineHeight
		};
	}

	std::string Hex(uint32_t value, uint8_t width) const
	{
		std::string s(width, '0');
		for (int32_t i = width - 1; i >= 0; --i, value >>= 4)
		{
			s[i] = "0123456789ABCDEF"[value & 0xF];
		}
		return s;
	}

	const RAM& mRAM;
	uint16_t mCurrentAddress;
	WidgetFrame mFrame;
};

//--------------------------------------------------------------------------------
class UIManager
{
	static constexpr int kSpacing = 4;
	enum class VertAlign { Top, Bottom };
	enum class HortAlign { Left, Right };

public:
	UIManager(const CPU& cpu, const Bus& bus)		
		: mRegisterUI(cpu)
		, mStackUI(cpu)
		, mDisplayUI(bus.mDisplay)
		, mKeypadUI(bus.mKeypad)
		, mMemoryUI(bus.mRAM)
	{
		// Register all widgets
		mWidgets = {
			&mStatusUI,
			&mCPUStateUI,
			&mRegisterUI,
			&mStackUI,
			&mDisplayUI,
			&mKeypadUI,
			&mMemoryUI,
			&mInstructionUI
		};

		const olc::vi2d start{ kSpacing, kSpacing };

		// Position the status strip first
		mStatusUI.SetPosition(start);

		// Position all other UI elements below the status strip
		const olc::vi2d belowStatus = start + olc::vi2d{ 0, mStatusUI.GetSize().y + kSpacing };

		mDisplayUI.SetPosition(belowStatus);
		PlaceRightOf(mKeypadUI, mDisplayUI, kSpacing, VertAlign::Top);
		PlaceBelow(mInstructionUI, mDisplayUI, kSpacing, HortAlign::Left);
		PlaceRightOf(mStackUI, mKeypadUI, kSpacing, VertAlign::Top);
		PlaceRightOf(mRegisterUI, mStackUI, kSpacing, VertAlign::Top);
		PlaceRightOf(mMemoryUI, mInstructionUI, kSpacing, VertAlign::Top);
		PlaceRightOf(mCPUStateUI, mMemoryUI, kSpacing, VertAlign::Top);

		// Special case		
		ResizeStatusToMatchContent();
	}

	void UpdateDisplay(const InstructionInfo& info, const CPUState& cpuState)
	{
		mInstructionUI.UpdateDisplay(info);
		mCPUStateUI.UpdateDisplay(cpuState, info.mPreviewCycle);
		mMemoryUI.SetAddress(info.mAddress);
	}	

	void SetStatusText(const std::string& text)
	{
		mStatusUI.SetText(text);
	}

	olc::vi2d GetCanvasSize() const
	{
		olc::vi2d widgetBounds = ComputeBoundsOf(mWidgets);
		return widgetBounds + olc::vi2d{ kSpacing, kSpacing };
	}

	void Draw(olc::PixelGameEngine& pge) const
	{
		mStatusUI.Draw(pge);
		mCPUStateUI.Draw(pge);
		mRegisterUI.Draw(pge);
		mStackUI.Draw(pge);
		mDisplayUI.Draw(pge);
		mKeypadUI.Draw(pge);
		mMemoryUI.Draw(pge);
		mInstructionUI.Draw(pge);
	}

private:
	std::vector<const IWidget*> GetWidgetsExcluding(std::initializer_list<const IWidget*> excludeList) const
	{
		std::vector<const IWidget*> result;

		for (const IWidget* widget : mWidgets)
		{
			if (std::find(excludeList.begin(), excludeList.end(), widget) == excludeList.end())
			{
				result.push_back(widget);
			}
		}

		return result;
	}

	void ResizeStatusToMatchContent()
	{
		// Compute layout bounds (excluding status), and resize status to match layout width
		const auto contentWidgets = GetWidgetsExcluding({ &mStatusUI });
		const olc::vi2d contentBounds = ComputeBoundsOf(contentWidgets);
		mStatusUI.SetWidth(contentBounds.x - 2 * kSpacing);
	}

	void PlaceRightOf(IWidget& target, const IWidget& anchor, int32_t spacing, VertAlign align)
	{
		const auto anchorPosition = anchor.GetPosition();
		const auto anchorSize = anchor.GetSize();
		const auto targetSize = target.GetSize();

		int32_t y = anchorPosition.y;
		if (align == VertAlign::Bottom)
		{
			y += anchorSize.y - targetSize.y;
		}

		target.SetPosition({ anchorPosition.x + anchorSize.x + spacing, y });
	}

	void PlaceBelow(IWidget& target, const IWidget& anchor, int32_t spacing, HortAlign align)
	{
		const auto anchorPosition = anchor.GetPosition();
		const auto anchorSize = anchor.GetSize();
		const auto targetSize = target.GetSize();

		int32_t x = anchorPosition.x;
		if (align == HortAlign::Right)
		{
			x += anchorSize.x - targetSize.x;
		}

		target.SetPosition({ x, anchorPosition.y + anchorSize.y + spacing });
	}
	
	olc::vi2d ComputeBoundsOf(const std::vector<const IWidget*>& widgets) const
	{
		olc::vi2d max = { 0, 0 };

		for (const IWidget* widget : widgets)
		{
			const olc::vi2d bottomRight = widget->GetPosition() + widget->GetSize();
			max.x = std::max(max.x, bottomRight.x);
			max.y = std::max(max.y, bottomRight.y);
		}

		return max;
	}

	StatusUI mStatusUI;
	CPUStateUI mCPUStateUI;
	RegisterUI mRegisterUI;
	StackUI mStackUI;
	DisplayUI mDisplayUI;
	KeypadUI mKeypadUI;
	MemoryUI mMemoryUI;
	InstructionUI mInstructionUI;
	std::vector<const IWidget*> mWidgets;
};

//--------------------------------------------------------------------------------
class Application : public olc::PixelGameEngine
{
public:
	Application()
		: mIsHalted(false)
		, mUIManager(mEmulator.GetCPU(), mEmulator.GetBus())
	{
		sAppName = "Chip8 Emulator";
		mUIManager.SetStatusText("Ready");
	}

	olc::vi2d GetCanvasSize() const { return mUIManager.GetCanvasSize(); }

	bool OnUserCreate() override
	{
		RomManager romManager(ROMS_PATH);
		const auto romPath = romManager.ResolveRom("test_rom/test_opcode.ch8");

		if (!mEmulator.LoadRom(romPath))
		{
			std::cerr << "Unable to load ROM: " << romPath << std::endl;
			return false;
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		mUIManager.Draw(*this);
		
		if (mIsHalted)
		{
			return true;
		}

		InstructionInfo info = mEmulator.PreviewInstruction();
		CPUState cpuState = mEmulator.GetCPU().GetState();

		mUIManager.UpdateDisplay(info, cpuState);
		LogInstructionPreview(info);

		if (!info.mDecodeSucceeded)
		{
			mUIManager.SetStatusText("Execution halted: Decode Error");
			mIsHalted = true;
			return true;
		}

		ExecutionStatus status = mEmulator.Step();
		assert(status != ExecutionStatus::DecodeError);

		if (status != ExecutionStatus::Executed)
		{
			mUIManager.SetStatusText("Execution halted: " + ExecutionStatusToString(status));
			mIsHalted = true;
		}
		
		LogInstructionExecution(info, status);
		
		return true;
	}

private:
	void LogHex(std::ostream& os, uint16_t value, int width = 4)
	{
		os << "0x"
			<< std::uppercase << std::hex << std::setw(width) << std::setfill('0') << value
			<< std::dec << std::nouppercase << std::setfill(' ');
	}

	void LogInstructionPreview(const InstructionInfo& info)
	{
		std::cout << "--- Cycle " << info.mPreviewCycle << " -----------------------------------\n";
		std::cout << "Fetch and decode opcode ";
		LogHex(std::cout, info.mOpcode);
		std::cout << " at ";
		LogHex(std::cout, info.mAddress);
		std::cout << std::endl;
	}

	void LogInstructionExecution(const InstructionInfo& info, ExecutionStatus status)
	{
		std::cout << "Execute opcode ";
		LogHex(std::cout, info.mOpcode);
		std::cout << " (" << info.mPattern << ")\n";

		std::cout << ExecutionStatusToString(status) << std::endl;
	}

	std::string ExecutionStatusToString(ExecutionStatus status) const
	{
		switch (status)
		{
			case ExecutionStatus::Executed:
				return "Opcode Executed";
			case ExecutionStatus::DecodeError:
				return "Opcode Decode Error";
			case ExecutionStatus::NotImplemented:
				return "Opcode Not Implemented";
			case ExecutionStatus::MissingHandler:
				return "Opcode Handler Missing";
			default: 
				return "Unknown";
		}
	}

private:
	Emulator mEmulator;
	bool mIsHalted;
	UIManager mUIManager;
};