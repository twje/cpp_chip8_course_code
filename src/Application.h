#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "IWidget.h"
#include "WidgetFrame.h"
#include "WidgetDecorators.h"
#include "RomManager.h"
#include "Emulator.h"
#include "ExecutionStatus.h"

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
		const std::string sampleLabel = " F: 0xFFFF"; // Longest possible line
		const int32_t charWidth = 8;
		const int32_t charHeight = 8;

		return {
			static_cast<int32_t>(sampleLabel.size()) * charWidth,
			STACK_SIZE * charHeight
		};
	}

	virtual olc::vi2d GetPosition() const override
	{
		return mFrame.GetPosition();
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mFrame.SetPosition(position);
	}

	// TODO: simplify
	virtual void Draw(olc::PixelGameEngine& pge) const override
	{		
		mFrame.Draw(pge);

		const int32_t lineHeight = 8;

		for (size_t i = 0; i < STACK_SIZE; ++i)
		{			
			const bool isTop = (i == mCPU.GetStackPointer());			
			const std::string cursor = isTop ? ">" : " ";

			olc::vi2d pos = mFrame.GetContentOffset() + olc::vi2d{ 0, static_cast<int32_t>(i) * lineHeight };
			std::string text = cursor + Hex(i, 1) + ": 0x" + Hex(mCPU.GetStackValueAt(i), 4);

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

		constexpr int32_t lineHeight = 8;  // TODO: Move to UI style

		for (size_t i = 0; i < NUM_REGISTERS; ++i)
		{
			olc::vi2d pos = mFrame.GetContentOffset() + olc::vi2d{0, static_cast<int32_t>(i) * lineHeight};
			std::string text = Hex(i, 1) + ": 0x" + Hex(mCPU.GetRegisterValueAt(i), 2);

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
class CPUStateUI : public IWidget
{
public:
	CPUStateUI(const CPU& cpu)
		: mCPU(cpu)
		, mCurrentPC(0)
		, mCurrentOpcode(0)
		, mFrame("CPU State")
	{ 
		mFrame.SetContentSize(GetInternalContentSize());
	}
	
	void SetCurrentInstruction(const Instruction& instruction)
	{
		mCurrentPC = instruction.GetAddress();
		mCurrentOpcode = instruction.GetOpcode();
		mCurrentPattern = GetOpcodePatternString(instruction.GetPatternId());
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
		
		const olc::vi2d start = mFrame.GetContentOffset();
		const int32_t lineHeight = 8;

		auto DrawLine = [&](int lineIndex, const std::string& text)
		{
			olc::vi2d pos = start + olc::vi2d{ 0, lineIndex * lineHeight };
			pge.DrawString(pos, text, UIStyle::kColorText);
		};

		const int labelWidth = 8;

		DrawLine(0, RightAlign("PC:", labelWidth) + " 0x" + Hex(mCurrentPC, 4));
		DrawLine(1, RightAlign("I:", labelWidth) + " 0x" + Hex(mCPU.GetIndexRegister(), 4));
		DrawLine(2, RightAlign("Opcode:", labelWidth) + " 0x" + Hex(mCurrentOpcode, 4));
		DrawLine(3, RightAlign("Pattern:", labelWidth) + " " + mCurrentPattern);
		DrawLine(4, RightAlign("Delay:", labelWidth) + " 0x" + Hex(mCPU.GetDelayTimer(), 2));
		DrawLine(5, RightAlign("Sound:", labelWidth) + " 0x" + Hex(mCPU.GetSoundTimer(), 2));
	}

private:
	olc::vi2d GetInternalContentSize() const
	{
		std::string longestSample = "Pattern: 0xFFFF";

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

	const CPU& mCPU;
	uint16_t mCurrentPC;
	uint16_t mCurrentOpcode;
	std::string mCurrentPattern;
	olc::vi2d mPosition;
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
			default:             return "?";
		}
	}

	const Keypad& mKeypad;
	WidgetFrame mFrame;
};

//--------------------------------------------------------------------------------
class UIManager
{
public:
	UIManager(const CPU& cpu, const Bus& bus)
		: mCPUStateUI(cpu)
		, mRegisterUI(cpu)
		, mStackUI(cpu)
		, mDisplayUI(bus.mDisplay)
		, mKeypadUI(bus.mKeypad)
	{
		const olc::vi2d start{ 4, 4 };
		const int spacing = 4;  // TODO: reference style for spacing

		olc::vi2d foo = start;
		mDisplayUI.SetPosition(start);
		
		foo.x += mDisplayUI.GetSize().x + spacing;
		mCPUStateUI.SetPosition(foo);

		foo.x += mCPUStateUI.GetSize().x + spacing;
		mRegisterUI.SetPosition(foo + olc::vi2d{ 0, 0 });
		
		foo.x += mRegisterUI.GetSize().x + spacing;
		mStackUI.SetPosition(foo);

		foo = start;
		foo.x += mDisplayUI.GetSize().x + spacing;
		foo.y += mCPUStateUI.GetSize().y + spacing;

		mKeypadUI.SetPosition(foo);
	}

	void SetCurrentInstruction(const Instruction& instruction)
	{
		mCPUStateUI.SetCurrentInstruction(instruction);
	}	

	void Draw(olc::PixelGameEngine& pge) const
	{
		mCPUStateUI.Draw(pge);
		mRegisterUI.Draw(pge);
		mStackUI.Draw(pge);
		mDisplayUI.Draw(pge);
		mKeypadUI.Draw(pge);
	}

private:
	CPUStateUI mCPUStateUI;
	RegisterUI mRegisterUI;
	StackUI mStackUI;
	DisplayUI mDisplayUI;
	KeypadUI mKeypadUI;
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
	}

	bool OnUserCreate() override
	{
		RomManager romManager(ROMS_PATH);
		const auto romPath = romManager.ResolveRom("test_rom/test_opcode.8o");

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
		//mCPUStateUI.Draw(*this);

		if (mIsHalted)
		{
			return true;
		}

		CPU& cpu = mEmulator.GetCPU();
		Instruction instruction = cpu.Fetch();
				
		// UI displays the address the instruction was read from (before PC was incremented),
		// not the current PC value after fetch.
		mUIManager.SetCurrentInstruction(instruction);

		LogCycle(instruction);

		if (!cpu.Decode(instruction))
		{
			std::cerr << "Decode error" << std::endl;
			mIsHalted = true;
			return true;
		}
			
		// After decode — update mnemonic
		mUIManager.SetCurrentInstruction(instruction);

		const ExecutionStatus status = cpu.Execute(instruction);
		LogExecution(instruction, status);
		mIsHalted = !ShouldContinue(status);
		
		return true;
	}

private:
	bool ShouldContinue(ExecutionStatus status)
	{
		switch (status)
		{
			case ExecutionStatus::Executed:
			case ExecutionStatus::Ignored:
				return true;
			default:
				return false;
		}
	}

	void LogHex(std::ostream& os, uint16_t value, int width = 4)
	{
		os << "0x"
			<< std::uppercase << std::hex << std::setw(width) << std::setfill('0') << value
			<< std::dec << std::nouppercase << std::setfill(' ');
	}

	void LogCycle(const Instruction& instruction)
	{
		static int cycle = 0;
		if (cycle > 0) { std::cout << std::endl; }

		std::cout << "--- Cycle " << cycle++ << " -----------------------------------" << std::endl;
		std::cout << "Fetch and decode opcode ";
		LogHex(std::cout, instruction.GetOpcode());
		std::cout << " at ";
		LogHex(std::cout, instruction.GetAddress());
		std::cout << std::endl;
	}

	void LogExecution(const Instruction& instruction, ExecutionStatus status)
	{
		std::cout << "Execute opcode ";
		LogHex(std::cout, instruction.GetOpcode());
		std::cout << " (" << GetOpcodePatternString(instruction.GetPatternId()) << ")" << std::endl;

		switch (status)
		{
			case ExecutionStatus::Executed:
				std::cout << "Executed" << std::endl; 
				break;
			case ExecutionStatus::Ignored:
				std::cout << "Ignored" << std::endl; 
				break;
			case ExecutionStatus::NotImplemented:
				std::cout << "Not implemented" << std::endl;
				break;
			case ExecutionStatus::MissingHandler:
				std::cout << "Missing handler" << std::endl;
				break;
		}
	}

private:	
	Emulator mEmulator;
	bool mIsHalted;

	UIManager mUIManager;
};