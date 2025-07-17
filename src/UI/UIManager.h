#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interfaces/IUIManager.h"
#include "UI/Layout/LayoutHelpers.h"
#include "Platform/Olc/OlcVec.h"
#include "UI/Widgets/ControlsWidget.h"
#include "UI/Widgets/NotificationWidget.h"
#include "UI/Widgets/RomListWidget.h"
#include "UI/Widgets/DisplayWidget.h"
#include "UI/Widgets/KeypadWidget.h"
#include "UI/Widgets/InstructionWidget.h"
#include "UI/Widgets/StackWidget.h"
#include "UI/Widgets/MemoryWidget.h"
#include "UI/Widgets/RegisterWidget.h"

// Third Party
#include "olcPixelGameEngine.h"

//--------------------------------------------------------------------------------
class UIManager : public IUIManager
{
public:
	UIManager(olc::PixelGameEngine& pge)
		: mControlsWidget(pge)
		, mNotificationWidget(pge)
		, mRomListWidget(pge)
		, mDisplayWidget(pge)
		, mKeypadWidget(pge)
		, mInstructionWidget(pge)
		, mStackWidget(pge)
		, mMemoryWidget(pge)
		, mRegisterWidget(pge)
	{
		LayoutTopWidgets();

		PlaceBelow(mRomListWidget, mNotificationWidget, UITheme::kWidgetSpacing, HortAlign::Left);
		PlaceBelow(mDisplayWidget, mRomListWidget, UITheme::kWidgetSpacing, HortAlign::Left);
		PlaceBelow(mKeypadWidget, mDisplayWidget, UITheme::kWidgetSpacing, HortAlign::Left);
		PlaceRightOf(mInstructionWidget, mKeypadWidget, UITheme::kWidgetSpacing, VertAlign::Top);
		PlaceLeftOf(mKeypadWidget, mInstructionWidget, UITheme::kWidgetSpacing, VertAlign::Bottom);
		PlaceRightOf(mStackWidget, mDisplayWidget, UITheme::kWidgetSpacing, VertAlign::Top);
		PlaceBelow(mMemoryWidget, mStackWidget, UITheme::kWidgetSpacing, HortAlign::Right);
		PlaceRightOf(mRegisterWidget, mStackWidget, UITheme::kWidgetSpacing, VertAlign::Top);
	}

	//---------------------
	// IUIManager Interface
	//---------------------
	virtual void SetCommandCallback(const std::function<void(Commands)>& callback) override
	{
		mControlsWidget.SetCommandCallback(callback);
	}

	virtual void SetCommandState(Commands command, bool enabled) override
	{
		mControlsWidget.EnableButton(command, enabled);
	}

	virtual void SetRomList(const std::vector<std::string>& roms) override
	{
		mRomListWidget.SetRomList(roms);
	}

	virtual void SetOnRomSelectedCallback(const std::function<void(size_t)>& callback) override
	{
		mRomListWidget.SetOnRomSelectedCallback(callback);
	}

	virtual void SelectRomAtIndex(size_t index) override
	{
		mRomListWidget.SelectRomAtIndex(index);
	}

	virtual void Update(float elapsedTime) override
	{
		mControlsWidget.Update(elapsedTime);
		mRomListWidget.Update(elapsedTime);
	}

	virtual void Draw(const ViewModel& viewModel) override
	{
		mControlsWidget.Draw(viewModel);
		mNotificationWidget.Draw(viewModel);
		mRomListWidget.Draw(viewModel);
		mDisplayWidget.Draw(viewModel);
		mKeypadWidget.Draw(viewModel);
		mInstructionWidget.Draw(viewModel);
		mStackWidget.Draw(viewModel);
		mMemoryWidget.Draw(viewModel);
		mRegisterWidget.Draw(viewModel);
	}

private:
	void LayoutTopWidgets()
	{
		const int32_t contentWidth = SCREEN_WIDTH - UITheme::kWidgetSpacing * 2;
		const olc::vi2d contentStart{ UITheme::kWidgetSpacing, UITheme::kWidgetSpacing };

		const int32_t controlsX = (contentWidth - mControlsWidget.GetSize().mX) / 2;
		const olc::vi2d controlsPosition{ controlsX, 0 };

		mControlsWidget.SetPosition(ToIntVec2(contentStart + controlsPosition));

		mNotificationWidget.SetWidth(contentWidth);
		mRomListWidget.SetWidth(contentWidth);

		const int32_t notifyWidgetY = mControlsWidget.GetSize().mY + UITheme::kWidgetSpacing;
		mNotificationWidget.SetPosition(ToIntVec2(contentStart + olc::vi2d{ 0, notifyWidgetY }));
	}

	ControlsWidget mControlsWidget;
	NotificationWidget mNotificationWidget;
	RomListWidget mRomListWidget;
	DisplayWidget mDisplayWidget;
	KeypadWidget mKeypadWidget;
	InstructionWidget mInstructionWidget;
	StackWidget mStackWidget;
	MemoryWidget mMemoryWidget;	
	RegisterWidget mRegisterWidget;
};