#pragma once

// Includes
//--------------------------------------------------------------------------------
// Domain
#include "Types/Commands.h"
#include "UI/ViewModel.h"

// System
#include <functional>
#include <string>
#include <vector>

//--------------------------------------------------------------------------------
class IUIManager
{
public:
	virtual ~IUIManager() = default;

	// Bindings & Callbacks
	virtual void SetCommandCallback(const std::function<void(Commands)>& callback) = 0;
	virtual void SetCommandState(Commands command, bool enabled) = 0;

	// State Configuration & UI Actions
	virtual void SetRomList(const std::vector<std::string>& roms) = 0;
	virtual void SetOnRomSelectedCallback(const std::function<void(size_t)>& callback) = 0;
	virtual void SelectRomAtIndex(size_t index) = 0;
	virtual void DisplayNotification(const std::string& message) = 0;

	// Per-Frame Operations
	virtual void Update(float elapsedTime) = 0;
	virtual void Draw(const ViewModel& viewModel) = 0;
};