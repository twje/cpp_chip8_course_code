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
	MOCK_METHOD(std::vector<uint8_t>, LoadRom, (std::string_view romName), (const, override));
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
		ON_CALL(*romLoader, LoadRom(testing::_)).WillByDefault([this](std::string_view) {
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

	const ViewModel& GetViewModel() const
	{
		return mController->mViewModel;
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

	void AssertNotificationEquals(const ViewModel& vm, const std::string& expected)
	{
		ASSERT_EQ(vm.mNotficationText, expected);
		ASSERT_FALSE(vm.mIsNotificationError);
	}

	void AssertNotificationHalted(const ViewModel& vm, ExecutionStatus expectedStatus)
	{
		ASSERT_THAT(vm.mNotficationText, testing::HasSubstr("Execution Halted"));
		ASSERT_TRUE(vm.mIsNotificationError);

		const std::string expectedSubstr = Strings::ExecutionStatusToString(expectedStatus);
		ASSERT_THAT(vm.mNotficationText, testing::HasSubstr(expectedSubstr));
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
		
	// Act
	auto driver = CreateAppControllerDriver(std::move(config));

	// Assert
	const auto& vm = driver->GetViewModel();

	ASSERT_EQ(driver->GetExecutionState(), ExecutionState::kWaitingForRom);
	AssertNotificationEquals(vm, Strings::Notifications::kPleaseSelectRom);
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

	// Act
	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);	

	// Assert
	const auto& vm = driver->GetViewModel();

	ASSERT_EQ(driver->GetExecutionState(), ExecutionState::kStepping);
	ASSERT_EQ(driver->GetSnapshot().mAddress, 0x0200);
	ASSERT_EQ(driver->GetSnapshot().mOpcode, 0x1001);
	AssertNotificationEquals(vm, Strings::Notifications::kWaitingForStepInput);
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
	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);

	// Act
	driver->HandleCommand(Commands::kStep);

	// Assert
	const auto& vm = driver->GetViewModel();

	ASSERT_EQ(driver->GetSnapshot().mCycleCount, 1);
	ASSERT_EQ(driver->GetSnapshot().mAddress, 0x0202);
	AssertNotificationEquals(vm, Strings::Notifications::kWaitingForStepInput);
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
	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);

	// Act
	driver->HandleCommand(Commands::kStep);

	// Assert
	const auto& vm = driver->GetViewModel();

	ASSERT_EQ(driver->GetExecutionState(), ExecutionState::kHalted);
	ASSERT_EQ(driver->GetSnapshot().mOpcode, 0xFFFF);
	AssertNotificationHalted(vm, ExecutionStatus::DecodeError);
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

	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);

	// Act
	driver->HandleCommand(Commands::kPlay);
	driver->RunFrame();

	// Assert
	const auto& vm = driver->GetViewModel();

	ASSERT_EQ(driver->GetExecutionState(), ExecutionState::kHalted);
	AssertNotificationHalted(vm, ExecutionStatus::DecodeError);
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

//--------------------------------------------------------------------------------
TEST_F(ApplicationControllerTestFixture, OutOfBoundsLowerAddress_ShouldHalt)
{
	/*
		Verifies execution halts if a jump targets a low out-of-bounds address.

		Checks:
		- Emulator transitions to Halted state.
		- Displays correct halt reason.
	*/

	// Arrange
	std::vector<uint8_t> romData = {
		0x11, 0xFE  // JP 0x1FE — invalid (below program range)
	};
	AppControllerDriverConfig config(romData);
	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);

	// Act
	driver->HandleCommand(Commands::kPlay);
	driver->RunFrame();

	// Assert
	const auto& vm = driver->GetViewModel();

	ASSERT_EQ(driver->GetExecutionState(), ExecutionState::kHalted);
	AssertNotificationHalted(vm, ExecutionStatus::InvalidAddressOutOfBounds);
}

//--------------------------------------------------------------------------------
TEST_F(ApplicationControllerTestFixture, OutOfBoundsUpperAddress_ShouldHalt)
{
	/*
		Verifies execution halts if an instruction attempts to read beyond memory bounds.

		Checks:
		- Emulator transitions to Halted state.
		- Displays correct halt reason.
	*/

	// Arrange
	std::vector<uint8_t> romData = {
		0x1F, 0xFE,  // JP 0x0FFE — valid (jumps to last valid instruction)
		0x00, 0xE0   // CLS — will be fetched from invalid memory (past 0x0FFF)
	};
	AppControllerDriverConfig config(romData);
	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);

	// Act
	driver->HandleCommand(Commands::kPlay);
	driver->RunFrame();

	// Assert
	const auto& vm = driver->GetViewModel();

	ASSERT_EQ(driver->GetExecutionState(), ExecutionState::kHalted);
	AssertNotificationHalted(vm, ExecutionStatus::InvalidAddressOutOfBounds);
}

//--------------------------------------------------------------------------------
TEST_F(ApplicationControllerTestFixture, UnalignedAddress_ShouldHalt)
{
	/*
		Verifies execution halts if a jump targets a misaligned (odd) address.

		Checks:
		- Emulator transitions to Halted state.
		- Displays correct halt reason.
	*/

	// Arrange
	std::vector<uint8_t> romData = {
		0x12, 0x23,  // JP 0x223 — invalid (unaligned address)
		0x00, 0xE0
	};
	AppControllerDriverConfig config(romData);
	auto driver = CreateAppControllerDriver(std::move(config));
	driver->SelectRom(0);

	// Act
	driver->HandleCommand(Commands::kPlay);
	driver->RunFrame();

	// Assert
	const auto& vm = driver->GetViewModel();

	ASSERT_EQ(driver->GetExecutionState(), ExecutionState::kHalted);
	AssertNotificationHalted(vm, ExecutionStatus::InvalidAddressUnaligned);
}