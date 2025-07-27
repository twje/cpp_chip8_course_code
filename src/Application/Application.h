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

                constexpr std::array<std::pair<uint8_t, olc::Key>, 16> defaultBindings = { {
			{ Key::Key1, olc::Key::K1 },
			{ Key::Key2, olc::Key::K2 },
			{ Key::Key3, olc::Key::K3 },
			{ Key::KeyC, olc::Key::K4 },
			{ Key::Key4, olc::Key::Q  },
			{ Key::Key5, olc::Key::W  },
			{ Key::Key6, olc::Key::E  },
			{ Key::KeyD, olc::Key::R  },
			{ Key::Key7, olc::Key::A  },
			{ Key::Key8, olc::Key::S  },
			{ Key::Key9, olc::Key::D  },
			{ Key::KeyE, olc::Key::F  },
			{ Key::KeyA, olc::Key::Z  },
			{ Key::Key0, olc::Key::X  },
			{ Key::KeyB, olc::Key::C  },
			{ Key::KeyF, olc::Key::V  },
		} };

		for (uint8_t i = 0; i < defaultBindings.size(); ++i)
		{
			const auto& [keyId, olcKey] = defaultBindings[i];
			keypad.SetKeyBinding(Key(keyId), static_cast<uint8_t>(olcKey));
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