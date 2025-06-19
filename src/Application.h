#pragma once

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "IWidget.h"
#include "WidgetDecorators.h"
#include "RomManager.h"
#include "Emulator.h"
#include "ExecutionStatus.h"

// Third Party
#include "olcPixelGameEngine.h"

// System
#include <string>

//--------------------------------------------------------------------------------
class Screen : public IWidget
{
public:
	Screen(Display& display)
		: mDisplay(display)
		, mPixelScale(1) // TODO: remove scale
	{ }

	virtual olc::vi2d GetSize() const override
	{
		return {
			static_cast<int32_t>(DISPLAY_WIDTH) * mPixelScale,
			static_cast<int32_t>(DISPLAY_HEIGHT) * mPixelScale
		};
	}

	virtual olc::vi2d GetPosition() const override
	{
		return mPosition;
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mPosition = position;
	}

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		const olc::vi2d pixelSize{ mPixelScale, mPixelScale };

		for (int y = 0; y < DISPLAY_HEIGHT; ++y)
		{
			for (int x = 0; x < DISPLAY_WIDTH; ++x)
			{
				olc::Pixel color = mDisplay.IsPixelSet(x, y)
					? olc::GREEN
					: olc::DARK_GREY;

				const olc::vi2d position = mPosition + olc::vi2d{ x, y } *mPixelScale;
				pge.FillRect(position, pixelSize, color);
			}
		}
	}

private:
	Display& mDisplay;
	olc::vi2d mPosition;
	const int32_t mPixelScale;
};

//--------------------------------------------------------------------------------
class StackDisplay : public IWidget
{
public:
	StackDisplay(CPU& cpu)
		: mCPU(cpu)
	{ }

	virtual olc::vi2d GetSize() const override
	{
		const std::string sampleLabel = "F: 0xFFFF"; // Longest possible line
		const int32_t charWidth = 8;
		const int32_t charHeight = 8;

		return {
			static_cast<int32_t>(sampleLabel.size()) * charWidth,
			STACK_SIZE * charHeight
		};
	}

	virtual olc::vi2d GetPosition() const override
	{
		return mPosition;
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mPosition = position;
	}

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		const int32_t lineHeight = 8;

		for (size_t i = 0; i < STACK_SIZE; ++i)
		{
			olc::vi2d linePos = mPosition + olc::vi2d{ 0, static_cast<int32_t>(i) * lineHeight };

			// Highlight the current top of the stack
			olc::Pixel textColor = (i == mCPU.GetStackPointer())
				? olc::RED
				: olc::WHITE;

			std::string entry = Hex(i, 1) + ": 0x" + Hex(mCPU.GetStackValueAt(i), 4);
			pge.DrawString(linePos, entry, textColor);
		}
	}

private:
	std::string Hex(uint32_t value, uint8_t width) const
	{
		std::string s(width, '0');
		for (int i = width - 1; i >= 0; --i, value >>= 4)
		{
			s[i] = "0123456789ABCDEF"[value & 0xF];
		}
		return s;
	}

	CPU& mCPU;
	olc::vi2d mPosition;
};

/*
//--------------------------------------------------------------------------------
class WidgetDecorator : public IWidget
{
public:
	WidgetDecorator(std::unique_ptr<IWidget> inner, std::string title)
		: mInner(std::move(inner)), mTitle(std::move(title))
	{ }

	virtual olc::vi2d GetSize() const override
	{
		// Add border thickness and title spacing
		return mInner->GetSize() + olc::vi2d{ 2 * mBorder, mTitleHeight };
	}

	virtual olc::vi2d GetPosition() const override
	{
		return mPosition;
	}

	virtual void SetPosition(const olc::vi2d& position) override
	{
		mPosition = position;

		// Inner widget is inset
		olc::vi2d innerPos = position + olc::vi2d{ mBorder, mBorder + mTitleHeight };
		mInner->SetPosition(innerPos);
	}

	virtual void Draw(olc::PixelGameEngine& pge) const override
	{
		pge.DrawRect(mPosition, GetSize(), olc::DARK_GREY);
		
		if (!mTitle.empty())
		{
			pge.DrawString(mPosition + olc::vi2d{ 2, 0 }, mTitle, olc::YELLOW);
		}
	
		mInner->Draw(pge);
	}

private:
	std::unique_ptr<IWidget> mInner;
	std::string mTitle;
	olc::vi2d mPosition;

	static constexpr int32_t mBorder = 1;
	static constexpr int32_t mTitleHeight = 10;
};
*/

//--------------------------------------------------------------------------------
class Application : public olc::PixelGameEngine
{
public:
	Application()
		: mIsHalted(false)
		, mScreen(mEmulator.GetBus().mDisplay)
		, mStackDisplay(mEmulator.GetCPU())
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

/*		auto stackDisplayer = std::make_unique<StackDisplay>(mEmulator.GetCPU());
		mStackDisplayV2 = std::make_unique<TitleDecorator>(std::move(stackDisplayer), "FOO");*/

		auto widget = std::make_unique<StackDisplay>(mEmulator.GetCPU());
		auto widget1 = std::make_unique<TitleDecorator>(std::move(widget), "Stack");
		mStackDisplayV2 = std::make_unique<BackgroundDecorator>(std::move(widget1), olc::VERY_DARK_GREY);
		
		
		//widget = std::make_unique<WidgetBorder>(std::move(widget));

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{		
		mStackDisplayV2->Draw(*this);

		if (mIsHalted)
		{
			return true;
		}

		CPU& cpu = mEmulator.GetCPU();
		Instruction instruction = cpu.Fetch();

		LogCycle(instruction);

		if (!cpu.Decode(instruction))
		{
			std::cerr << "Decode error" << std::endl;
			mIsHalted = true;
		}
			
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
		std::cout << " (" << ToString(instruction.GetPatternId()) << ")" << std::endl;

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
	Screen mScreen;
	StackDisplay mStackDisplay;

	std::unique_ptr<IWidget> mStackDisplayV2;
};