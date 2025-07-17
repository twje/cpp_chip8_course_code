#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Application/Timer.h"
#include "Application/TextSpinner.h"
#include "Interfaces/IUIManager.h"
#include "Interfaces/IRomLoader.h"
#include "Types/Commands.h"
#include "Interpreter/Interpreter.h"
#include "Utils/TextUtils.h"
#include "Strings.h"

// System
#include <functional>
#include <iostream>
#include <string>
#include <vector>

//--------------------------------------------------------------------------------
class ApplicationController
{
#ifdef UNIT_TESTING
	friend class ApplicationControllerTestDriver;
#endif

public:
	ApplicationController(std::unique_ptr<IRomLoader> romLoader, std::unique_ptr<IUIManager> uiManager)
		: mRomLoader(std::move(romLoader))
		, mUIManager(std::move(uiManager))
		, mState(ExecutionState::kNone)
		, mInstructionTimer(CPU_FREQUENCY_HZ)
		, mSystemTimer(SYSTEM_TIMER_HZ)
		, mTextSpinner({ ".", "..", "..." }, 0.5f)
	{ }

	bool Initialize(std::unique_ptr<IKeyInputProvider> inputProvider)
	{
		// Bind runtime dependencies
		mViewModel.mBus = &mInterpreter.GetBus();
		mInterpreter.GetBus().mKeypad.SetInputProvider(std::move(inputProvider));

		// Set initial execution state and show ROM prompt
		TransitionState(ExecutionState::kWaitingForRom);
		DisplayNotification(Strings::Notifications::kPleaseSelectRom, false);

		// Initialize UI with ROM list
		mUIManager->SetRomList(mRomLoader->GetRoms());

		// Register ROM selection callback
		mUIManager->SetOnRomSelectedCallback([&](size_t index) {
			mInterpreter.Reset();

			const auto& roms = mRomLoader->GetRoms();
			assert(index < roms.size() && "ROM index out of bounds");

			const bool success = TrySelectRomByName(roms[index]);
			if (success)
			{
				TransitionState(ExecutionState::kStepping);
				DisplayNotification(Strings::Notifications::kWaitingForStepInput, false);
			}
			else
			{
				TransitionState(ExecutionState::kWaitingForRom);
				DisplayNotification("Unable to load ROM: " + roms[index], true);
			}
		});

		// Register command handler callback
		mUIManager->SetCommandCallback([&](Commands command) {
			assert(IsCommandAllowed(command) && "Command is not allowed.");
			HandleCommand(command);
		});

		return true;
	}

	bool RunFrame(float elapsedTime)
	{		
		// Update
		mUIManager->Update(elapsedTime);		
		PollInput();		
		TickExecution(elapsedTime);
		UpdateSystemTimer(elapsedTime);
		
		// Draw
		mUIManager->Draw(mViewModel);
		
		return true;
	}

	Keypad& GetKeypad()
	{
		return mInterpreter.GetBus().mKeypad;
	}

private:
	//-----------------
	// Command Handling
	//-----------------
	void HandleCommand(Commands command)
	{
		switch (command)
		{
			case Commands::kPlay: 
				OnPlayCommand(); 
				break;
			case Commands::kPause:  
				OnPauseCommand(); 
				break;
			case Commands::kStep: 
				OnStepCommand(); 
				break;
			case Commands::kReset: 
				OnResetCommand(); 
				break;
			default:
				assert(false && "Unhandled command");
				break;
		}
	}

	void OnPlayCommand()
	{
		TransitionState(ExecutionState::kPlaying);
		DisplayNotification(Strings::Notifications::kRunning, false);
	}

	void OnPauseCommand()
	{
		TransitionState(ExecutionState::kStepping);
		DisplayNotification(Strings::Notifications::kWaitingForStepInput, false);
	}

	void OnStepCommand()
	{
		if (mState != ExecutionState::kStepping)
		{
			assert(false && "Step command is only valid in Stepping state");
			return;
		}

		if (ExecuteStep())
		{
			CaptureNextInstruction();
		}
	}

	void OnResetCommand()
	{
		mInterpreter.Reset();
		TransitionState(ExecutionState::kStepping);
		DisplayNotification(Strings::Notifications::kWaitingForStepInput, false);
	}

	//-------------------
	// Execution & Timers
	//-------------------
	void TickExecution(float elapsedTime)
	{
		if (mState != ExecutionState::kPlaying)
		{
			return;
		}
		
		// Append animated spinner to the "Running" notification
		std::string notification = std::format("{}{}", Strings::Notifications::kRunning, mTextSpinner.Update(elapsedTime));
		DisplayNotification(notification, false);

		for (size_t i = 0; i < mInstructionTimer.ComputeStepCount(elapsedTime); i++)
		{
			if (!ExecuteStep())
			{
				break;
			}
		}
	}

	void UpdateSystemTimer(float elapsedTime)
	{
		for (size_t i = 0; i < mSystemTimer.ComputeStepCount(elapsedTime); i++)
		{
			mInterpreter.DecrementTimers();
		}
	}

	bool ExecuteStep()
	{
		PollInput();

		const StepResult result = mInterpreter.Step();
		if (result.mShouldHalt)
		{
			TransitionState(ExecutionState::kHalted);
			DisplayNotification("Execution Halted (" + Strings::ExecutionStatusToString(result.mStatus) + ")", true);
			return false;
		}

		return true;
	}

	//-----------------
	// UI & Interaction
	//-----------------
	void PollInput()
	{
		mInterpreter.GetBus().mKeypad.PollKeypad();
	}
	
	void CaptureNextInstruction()
	{
		mViewModel.mSnapshot = mInterpreter.PeekNextInstruction();
		PrintInstruction(mViewModel.mSnapshot);
	}

	void SyncViewModel()  // TODO: comor back to
	{
		const bool isSteppingOrHalted = (mState == ExecutionState::kStepping || mState == ExecutionState::kHalted);
		mViewModel.mIsDisplayInteractive = isSteppingOrHalted;
	}

	void PrintInstruction(const Snapshot& snapshot) const
	{
		std::cout << "--- Cycle " << snapshot.mCycleCount << " -----------------------------------\n";
		std::cout << "Fetch opcode\n"
			<< "  Address : 0x" << ToHexString(snapshot.mAddress, 4) << "\n"
			<< "  Opcode  : 0x" << ToHexString(snapshot.mOpcode, 4) << "\n"
			<< "  Pattern : " << (snapshot.mDecodeSucceeded ? snapshot.mPattern : "-") << "\n"
			<< "  Mnemonic: " << (snapshot.mDecodeSucceeded ? snapshot.mMnemonic : "-") << "\n\n";
	}

	void DisplayNotification(const std::string& message, bool isError)
	{
		mViewModel.mNotficationText = message;
		mViewModel.mIsNotificationError = isError;
	}

	//---------------
	// ROM Management
	//---------------
	bool TrySelectRomByName(const std::string& romName)
	{
		return mInterpreter.LoadRom(mRomLoader->LoadRom(romName));
	}

	//-----------------
	// State Management
	//-----------------
	void TransitionState(ExecutionState newState)
	{
		mState = newState;
		std::cout << "Transitioning to state: " << Strings::ExecutionStateToString(mState) << std::endl;

		for (Commands cmd : { Commands::kPlay, Commands::kPause, Commands::kStep, Commands::kReset })
		{
			mUIManager->SetCommandState(cmd, IsCommandAllowed(cmd));
		}

		// Show next instruction for:
		// 1. Stepping - preview before execution.
		// 2. Halted - display the faulting instruction.
		if (mState == ExecutionState::kStepping || mState == ExecutionState::kHalted)
		{
			CaptureNextInstruction();
		}
	
		SyncViewModel();
	}

	bool IsCommandAllowed(Commands command) const
	{
		switch (mState)
		{
			case ExecutionState::kStepping:
				return command == Commands::kPlay || command == Commands::kStep || command == Commands::kReset;
			case ExecutionState::kPlaying:
				return command == Commands::kPause || command == Commands::kReset;
			case ExecutionState::kHalted:
				return command == Commands::kReset;
			default:
				return false;
		}
	}

	// Core execution
	Interpreter mInterpreter;
	ExecutionState mState;

	// Dependencies
	std::unique_ptr<IRomLoader> mRomLoader;
	std::unique_ptr<IUIManager> mUIManager;

	// UI binding
	ViewModel mViewModel;

	// Timers
	Timer mInstructionTimer;
	Timer mSystemTimer;
	TextSpinner mTextSpinner;
};