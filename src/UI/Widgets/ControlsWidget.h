#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Types/Commands.h"
#include "Constants.h"
#include "olcPixelGameEngine.h"
#include "extensions/olcPGEX_QuickGUI.h"

// Third Party
#include "Interfaces/IWidget.h"

// System
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

//--------------------------------------------------------------------------------
using Button = olc::QuickGUI::Button;

//--------------------------------------------------------------------------------
enum class ButtonId
{
	PLAY,
	PAUSE,	
	STEP,
	RESET
};

//--------------------------------------------------------------------------------
struct ButtonEntry
{
	Button* mButton = nullptr;
	Commands mCommand = Commands::NONE;
};

//--------------------------------------------------------------------------------
class ControlsWidget : public IWidget
{
	static constexpr olc::vi2d kButtonSize{ 111, 16 };

public:
	using CommandCallback = std::function<void(Commands)>;

	ControlsWidget(olc::PixelGameEngine& pge)
		: mPge(pge)
	{
		AddButton(ButtonId::PLAY, "Play", Commands::PLAY);
		AddButton(ButtonId::PAUSE, "Pause", Commands::PAUSE);
		AddButton(ButtonId::STEP, "Step", Commands::STEP);
		AddButton(ButtonId::RESET, "Reset", Commands::RESET);

		LayoutButtons();
	}

	void SetCommandCallback(const CommandCallback& callback)
	{
		mCommandCallback = callback;
	}

	void EnableButton(Commands command, bool enabled)
	{
		for (auto& [buttonId, entry] : mButtons)
		{
			if (entry.mCommand == command)
			{
				entry.mButton->Enable(enabled);
				if (!enabled)
				{
					entry.mButton->bPressed = false;  // Sticky input should be cleared when disabling (workaround)
				}
				break;
			}
		}
	}

	virtual IntVec2 GetSize() const override
	{
		return { static_cast<int32_t>(mButtonOrder.size()) * kButtonSize.x, kButtonSize.y };
	}

	virtual IntVec2 GetPosition() const override
	{
		return { mPosition.x, mPosition.y };
	}

	virtual void SetPosition(const IntVec2& position) override
	{
		mPosition = olc::vi2d{ position.mX, position.mY };
		LayoutButtons();
	}

	virtual void Update(float elapsedTime) override	
	{
		(void)elapsedTime; // Unused parameter
		
		HandleInput();
		mGuiManager.Update(&mPge);
	}	

	virtual void Draw(const ViewModel& viewModel) override
	{
		(void)viewModel; // Unused parameter	

		mGuiManager.DrawDecal(&mPge);
	}

private:
	void AddButton(ButtonId id, const std::string& text, Commands command)
	{
		Button* button = new Button(mGuiManager, text, { }, kButtonSize);
		mButtons[id] = { button, command };
		mButtonOrder.push_back(id);
	}

	void LayoutButtons()
	{
		for (size_t i = 0; i < mButtonOrder.size(); ++i)
		{
			ButtonId id = mButtonOrder[i];
			auto& entry = mButtons.at(id);
			entry.mButton->vPos = mPosition + olc::vi2d{ static_cast<int32_t>(i) * kButtonSize.x, 0 };
		}
	}

	void HandleInput()
	{
		for (ButtonId buttonId : mButtonOrder)
		{
			auto& entry = mButtons.at(buttonId);
			if (entry.mButton->bVisible && entry.mButton->bPressed && mCommandCallback)
			{
				mCommandCallback(entry.mCommand);
			}
		}
	}

	olc::PixelGameEngine& mPge;
	std::unordered_map<ButtonId, ButtonEntry> mButtons;
	std::vector<ButtonId> mButtonOrder;

	olc::QuickGUI::Manager mGuiManager;
	olc::vi2d mPosition;

	CommandCallback mCommandCallback;
};
