#pragma once

// Includes
//--------------------------------------------------------------------------------
// Core Application
#include "Application/RomLoader.h"
#include "Application/ApplicationController.h"
#include "Types/ExecutionState.h"
#include "Platform/Olc/OlcVec.h"
#include "Platform/Olc/OlcKeyInputProvider.h"
#include "UI/UIManager.h"

// Third Party
#include "olcPixelGameEngine.h"

// System
#include <memory>
#include <array>
#include <cstdint>

//--------------------------------------------------------------------------------
class Application : public olc::PixelGameEngine
{	
public:
	Application()
	{ 
		sAppName = "";
		
		mController = std::make_unique<ApplicationController>(			
			std::make_unique<RomLoader>(ROMS_PATH),
			std::make_unique<UIManager>(*this)
		);
	}

	bool OnUserCreate() override
	{	
		Keypad& keypad = mController->GetKeypad();

		const std::array<std::pair<Keypad::Key, olc::Key>, 16> defaultBindings = { {
			{ Keypad::Key::Key1, olc::Key::K1 },
			{ Keypad::Key::Key2, olc::Key::K2 },
			{ Keypad::Key::Key3, olc::Key::K3 },
			{ Keypad::Key::KeyC, olc::Key::K4 },
			{ Keypad::Key::Key4, olc::Key::Q  },
			{ Keypad::Key::Key5, olc::Key::W  },
			{ Keypad::Key::Key6, olc::Key::E  },
			{ Keypad::Key::KeyD, olc::Key::R  },
			{ Keypad::Key::Key7, olc::Key::A  },
			{ Keypad::Key::Key8, olc::Key::S  },
			{ Keypad::Key::Key9, olc::Key::D  },
			{ Keypad::Key::KeyE, olc::Key::F  },
			{ Keypad::Key::KeyA, olc::Key::Z  },
			{ Keypad::Key::Key0, olc::Key::X  },
			{ Keypad::Key::KeyB, olc::Key::C  },
			{ Keypad::Key::KeyF, olc::Key::V  },
		} };

		for (uint8_t i = 0; i < defaultBindings.size(); ++i)
		{
			const auto& [key, olcKey] = defaultBindings[i];
			keypad.SetKeyBinding(key, static_cast<uint8_t>(olcKey));
		}

		return mController->Initialize(std::make_unique<OlcKeyInputProvider>(*this));
	}

	bool OnUserUpdate(float elapsedTime) override
	{
		Clear(olc::Pixel(UITheme::kBackgroundColor));		
		return mController->RunFrame(elapsedTime);
	}

private:
	std::unique_ptr<ApplicationController> mController;	
};