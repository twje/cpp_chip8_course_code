#define UNIT_TESTING

/*
	ApplicationController Integration Tests

	These tests verify interactions between ApplicationController, UI, and ROM loader
	using Google Test and Google Mock.

	Notes:
	- Mocks isolate external dependencies.
	- InSequence is used to enforce notification order where needed.
	- Focused on ApplicationController integration, not interpreter internals.
*/

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Application/Application.h"
#include "Application/Strings.h"
#include "UI/ViewModel.h"
#include "Interfaces/IKeyInputProvider.h"

// Third Party
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Google Test Aliases
//--------------------------------------------------------------------------------
using ::testing::_;
using ::testing::ReturnRef;
using ::testing::Invoke;
using ::testing::NiceMock;

//--------------------------------------------------------------------------------
class DummyKeyInputProvider : public IKeyInputProvider
{
public:
	bool IsKeyPressed(uint8_t) const override { return false; }
};

//--------------------------------------------------------------------------------
class MockRomLoader : public IRomLoader
{
public:
	MOCK_METHOD(size_t, RomCount, (), (const, override));
	MOCK_METHOD(const std::vector<std::string>&, GetRoms, (), (const, override));
	MOCK_METHOD(std::vector<uint8_t>, LoadRom, (const std::string& romName), (const, override));
};

//--------------------------------------------------------------------------------
class MockUIManager : public IUIManager
{
public:
	MOCK_METHOD(void, SetCommandCallback, (const std::function<void(Commands)>&), (override));
	MOCK_METHOD(void, SetCommandState, (Commands, bool), (override));
	MOCK_METHOD(void, SetRomList, (const std::vector<std::string>&), (override));
	MOCK_METHOD(void, SetOnRomSelectedCallback, (const std::function<void(size_t)>&), (override));
	MOCK_METHOD(void, SelectRomAtIndex, (size_t), (override));
	MOCK_METHOD(void, DisplayNotification, (const std::string&), (override));
	MOCK_METHOD(void, Update, (float), (override));
	MOCK_METHOD(void, Draw, (const ViewModel&), (override));
};

//--------------------------------------------------------------------------------
struct AppControllerDriverConfig
{
	AppControllerDriverConfig(const std::vector<uint8_t>& romData)
		: mRomData(romData)
		, mUIManager(std::make_unique<NiceMock<MockUIManager>>())
	{ }

	std::vector<uint8_t> mRomData;
	std::unique_ptr<NiceMock<MockUIManager>> mUIManager;
};

//--------------------------------------------------------------------------------
class ApplicationControllerTestDriver
{
public:
	explicit ApplicationControllerTestDriver(AppControllerDriverConfig&& config)
		: mRomData(std::move(config.mRomData))
		, mRomList({ "dummy.rom" })  // Only one dummy ROM for testing
		, mUIManager(config.mUIManager.get())
	{
		auto romLoader = std::make_unique<NiceMock<MockRomLoader>>();
		ON_CALL(*romLoader, GetRoms()).WillByDefault(ReturnRef(mRomList));
		ON_CALL(*romLoader, LoadRom(testing::_)).WillByDefault([this](const std::string&) {
			return mRomData;
		});

		EXPECT_CALL(*mUIManager, SetOnRomSelectedCallback(_))
			.WillOnce(testing::SaveArg<0>(&mRomSelectCallback));

		mController = std::make_unique<ApplicationController>(			
			std::move(romLoader),
			std::move(config.mUIManager)			
		);

		mController->Initialize(std::make_unique<DummyKeyInputProvider>());
	}

	void SelectRom(size_t index)
	{
		ASSERT_TRUE(mRomSelectCallback);
		mRomSelectCallback(index);
	}
	
	void HandleCommand(Commands command)
	{
		mController->HandleCommand(command);
	}

	void RunFrame()
	{
		float secondsPerFrame = 1.0f / 60.0f; // Simulate 60 FPS
		mController->RunFrame(secondsPerFrame);
	}

	MockUIManager& GetUIManager() { return *mUIManager; }
	ExecutionState GetExecutionState() { return mController->mState; }
	const Snapshot& GetSnapshot() { return mController->mViewModel.mSnapshot; }

private:
	std::unique_ptr<ApplicationController> mController;
	MockUIManager* mUIManager; // Non-owning
	std::function<void(size_t)> mRomSelectCallback;

	std::vector<uint8_t> mRomData;
	std::vector<std::string> mRomList;
};

//--------------------------------------------------------------------------------
class ApplicationControllerTestFixture : public ::testing::Test
{
protected:
	std::unique_ptr<ApplicationControllerTestDriver> CreateAppControllerDriver(AppControllerDriverConfig&& config)
	{
		return std::make_unique<ApplicationControllerTestDriver>(std::move(config));
	}	
};

//--------------------------------------------------------------------------------
TEST_F(ApplicationControllerTestFixture, Initialize_ShouldShowWaitingForRom)
{
	/*
		Verifies initial state after controller initialization.

		Checks:
		- Displays "Please select ROM" notification.
		- Starts in WaitingForRom state.
	*/

	// Arrange
	std::vector<uint8_t> romData = { };
	AppControllerDriverConfig config(romData);
	EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kPleaseSelectRom));
		
	// Act
	auto driver = CreateAppControllerDriver(std::move(config));

	// Assert
	ASSERT_EQ(driver->GetExecutionState(), ExecutionState::kWaitingForRom);
}

//--------------------------------------------------------------------------------
TEST_F(ApplicationControllerTestFixture, SelectRom_ShouldLoadRomAndEnterSteppingState)
{
	/*
		Verifies ROM selection transitions to Stepping state.

		Checks:
		- Displays correct notifications.
		- Sets correct state and instruction info.
	*/

	// Arrange
	std::vector<uint8_t> romData{ 0x10, 0x01 };  // Dummy ROM with one instruction
	AppControllerDriverConfig config(romData);
		
	{
		testing::InSequence seq;
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kPleaseSelectRom));
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kWaitingForStepInput));
	}

	// Act
	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);	

	// Assert
	ASSERT_EQ(driver->GetExecutionState(), ExecutionState::kStepping);
	ASSERT_EQ(driver->GetSnapshot().mAddress, 0x0200);
	ASSERT_EQ(driver->GetSnapshot().mOpcode, 0x1001);
}

//--------------------------------------------------------------------------------
TEST_F(ApplicationControllerTestFixture, StepInstruction_ShouldExecuteAndUpdateInstructionInfo)
{
	/*
		Verifies stepping executes instruction and updates UI.

		Checks:
		- Execution proceeds normally.
		- Instruction info updates correctly.
	*/

	// Arrange
	std::vector<uint8_t> romData = { 0x60, 0x01, 0x00, 0xE0 };  // LD V0, 1; CLS
	AppControllerDriverConfig config(romData);
	{
		testing::InSequence seq;
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kPleaseSelectRom));
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kWaitingForStepInput));
	}
	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);

	// Act
	driver->HandleCommand(Commands::kStep);

	// Assert
	ASSERT_EQ(driver->GetSnapshot().mCycleCount, 1);
	ASSERT_EQ(driver->GetSnapshot().mAddress, 0x0202);
}

//--------------------------------------------------------------------------------
TEST_F(ApplicationControllerTestFixture, InvalidInstruction_ShouldHaltAndShowInstruction)
{
	/*
		Verifies invalid instruction halts execution.

		Checks:
		- Transitions to Halted state.
		- Displays halted instruction.
	*/

	// Arrange
	std::vector<uint8_t> romData = { 0xFF, 0xFF };  // Invalid opcode
	AppControllerDriverConfig config(romData);
	{
		testing::InSequence seq;
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kPleaseSelectRom));
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kWaitingForStepInput));
		EXPECT_CALL(*config.mUIManager, DisplayNotification(testing::StartsWith("Execution Halted")));
	}
	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);

	// Act
	driver->HandleCommand(Commands::kStep);

	// Assert
	ASSERT_EQ(driver->GetExecutionState(), ExecutionState::kHalted);
	ASSERT_EQ(driver->GetSnapshot().mOpcode, 0xFFFF);
}

//--------------------------------------------------------------------------------
TEST_F(ApplicationControllerTestFixture, Play_ShouldHaltWhenInvalidInstructionExecuted)
{
	/*
		Verifies emulator halts during Play if invalid instruction encountered.

		Checks:
		- Transitions to Halted state after running.
	*/

	// Arrange
	std::vector<uint8_t> romData = { 0xFF, 0xFF };  // Invalid opcode
	AppControllerDriverConfig config(romData);
	{
		testing::InSequence seq;
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kPleaseSelectRom));
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kWaitingForStepInput));
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kRunning));
		EXPECT_CALL(*config.mUIManager, DisplayNotification(testing::StartsWith("Execution Halted")));
	}
	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);

	// Act
	driver->HandleCommand(Commands::kPlay);
	driver->RunFrame();

	// Assert
	ASSERT_EQ(driver->GetExecutionState(), ExecutionState::kHalted);
}

//--------------------------------------------------------------------------------
TEST_F(ApplicationControllerTestFixture, InstructionInfo_ShouldRemainFrozenWhilePlaying)
{
	/*
		Verifies instruction info doesn't update while playing but updates after pausing.

		Checks:
		- Instruction info stays frozen during Play.
		- Info updates after Pause.
	*/

	// Arrange
	std::vector<uint8_t> romData = {
		0x60, 0x01,  // LD V0, 1
		0x61, 0x02,  // LD V1, 2
		0x12, 0x02   // JP 0x202 (loop)
	};
	AppControllerDriverConfig config(romData);
	{
		testing::InSequence seq;
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kPleaseSelectRom));
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kWaitingForStepInput));
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kRunning));
		EXPECT_CALL(*config.mUIManager, DisplayNotification(Strings::Notifications::kWaitingForStepInput));
	}
	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);

	// Act: Capture instruction info before Play
	const auto cycleBeforePlay = driver->GetSnapshot().mCycleCount;

	// Act: Play and run emulator frame
	driver->HandleCommand(Commands::kPlay);
	driver->RunFrame();

	// Assert: Instruction info frozen during Play
	ASSERT_EQ(driver->GetSnapshot().mCycleCount, cycleBeforePlay);

	// Act: Pause emulator
	driver->HandleCommand(Commands::kPause);

	// Assert: Instruction info updates after Pause
	ASSERT_GT(driver->GetSnapshot().mCycleCount, cycleBeforePlay);
}