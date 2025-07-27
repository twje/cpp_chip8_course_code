#define UNIT_TESTING

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interfaces/IRandomProvider.h"
#include "Interfaces/IKeyInputProvider.h"
#include "Constants.h"
#include "Interpreter/Interpreter.h"
#include "Interpreter/Bus.h"
#include "Types/ExecutionStatus.h"
#include "Interpreter/Hardware/RAM.h"
#include "Interpreter/Hardware/CPU.h"

// Thir Party
#include <gtest/gtest.h>
#include <gmock/gmock.h>

//--------------------------------------------------------------------------------
class MockRandomProvider : public IRandomProvider
{
public:
    MOCK_METHOD(uint8_t, GetRandomByte, (), (override));
};

//--------------------------------------------------------------------------------
class DisplayTestAccessor
{
public:
    DisplayTestAccessor(Display& display)
        : mDisplay(display)
    { }

    void SetPixel(uint32_t px, uint32_t py, bool value)
    {
		mDisplay.SetPixel(px, py, value);
    }

    bool IsPixelSet(uint32_t px, uint32_t py) const
    {
		return mDisplay.IsPixelSet(px, py);
    }

    void Clear()
    {
		mDisplay.Clear();
    }

private:
    Display& mDisplay;
};

//--------------------------------------------------------------------------------
class OpcodeTest : public ::testing::Test
{
protected:
    static constexpr uint8_t kMockedRandomValue = 0xAB;
    
    OpcodeTest()
		: mInterpreter(mRandomProvider)
    { }

    void SetUp() override
    {
        // Set default return value for GetRandomByte
        ON_CALL(mRandomProvider, GetRandomByte).WillByDefault(::testing::Return(kMockedRandomValue));
    }

    void WriteByteToMemory(uint16_t address, uint8_t byte)
    {
        RAM& ram = mInterpreter.mBus.mRAM;
        ram.Write(address, byte);
    }

    void WriteOpcodeAndSetPC(uint16_t address, uint16_t opcode)
    {
        RAM& ram = mInterpreter.mBus.mRAM;
        CPU& cpu = mInterpreter.mCPU;

        // Write opcode in big-endian format and set PC to its address
        ram.Write(address + 0, opcode >> 8);   // Write high byte first
        ram.Write(address + 1, opcode & 0xFF); // Write low byte second
        cpu.mState.mProgramCounter = address;
    }

    uint8_t ReadByteFromMemory(uint16_t address)
    {
        RAM& ram = mInterpreter.mBus.mRAM;
        return ram.Read(address);
    }

    Instruction ExecuteInstruction()
    {
        return ExecuteInstruction(ExecutionStatus::Executed);
    }

    Instruction ExecuteInstruction(ExecutionStatus expectedStatus)
    {
        CPU& cpu = mInterpreter.mCPU;
		
		// Peek next instruction (no side effects)
        const FetchResult fetchResult = cpu.Peek();
        EXPECT_TRUE(fetchResult.mIsValidAddress) << "Peek failed.";
        
        Instruction instruction = cpu.Decode(fetchResult.mOpcode);
        EXPECT_TRUE(instruction.IsValid()) << "Decode failed.";
        
		// Step (increments program counter)
        StepResult stepResult = mInterpreter.Step();
        EXPECT_EQ(expectedStatus, stepResult.mStatus) << "Unexpected execution status.";
        EXPECT_FALSE(stepResult.mShouldHalt) 
            << "Interpreter unexpectedly requested halt after execution.";

        return instruction;
    }

    // Expose internal state for test access only; this class is declared as a friend.
    CPUState& GetCPUStateRef() { return mInterpreter.mCPU.mState; }
    Bus& GetBusRef() { return mInterpreter.mBus; }

	MockRandomProvider mRandomProvider;	
    Interpreter mInterpreter;
};

// Clear the display.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 00E0_CLS)
{
    // -- Arrange --
    const uint16_t opcode = 0x00E0;
    
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);

    DisplayTestAccessor display{ GetBusRef().mDisplay };
    display.SetPixel(0, 0, true); // Pre-fill pixel to verify clear

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_FALSE(display.IsPixelSet(0, 0));
}

// Return from a subroutine.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 00EE_RET)
{    
    // -- Arrange --
    const uint16_t returnAddress = PROGRAM_START_ADDRESS + 10;    
    const uint16_t opcode = 0x00EE;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mStack[0] = returnAddress;
    GetCPUStateRef().mStackPointer = 1;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(0, GetCPUStateRef().mStackPointer);
    ASSERT_EQ(returnAddress, GetCPUStateRef().mProgramCounter);
}

// Jump to location nnn.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 1nnn_JP_ADDR)
{
    // -- Arrange --
    const uint16_t address = 0x024E;
    const uint16_t opcode = 0x1000 | address;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(address, GetCPUStateRef().mProgramCounter);    
}

// Call subroutine at nnn
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 2nnn_CALL_ADDR)
{
    // -- Arrange --
    const uint16_t address = 0x0F23;
    const uint16_t opcode = 0x2000 | address;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    // PC is incremented during fetch, so '+ INSTRUCTION_SIZE' gives the return address to push.
    const uint16_t returnAddress = PROGRAM_START_ADDRESS + INSTRUCTION_SIZE;

    ASSERT_EQ(1, GetCPUStateRef().mStackPointer);
    ASSERT_EQ(returnAddress, GetCPUStateRef().mStack[0]);
    ASSERT_EQ(address, GetCPUStateRef().mProgramCounter);
}

// Skip next instruction if Vx = kk.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 3xkk_SE_VX_KK)
{
    const uint16_t baseOpcode = 0x3000;
    const uint8_t vxReg = 2;
    const uint8_t matchValue = 0x05;

    // Test 1: Vx == kk -> skip
    {
		// -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8) | matchValue;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = matchValue;

        // -- Act --
        ExecuteInstruction();

		// -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 2: Vx != kk -> no skip
    {
        // -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8) | matchValue;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = 0x04;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

        // -- Reset --
        mInterpreter.Reset();
    }
}

// Skip next instruction if Vx != kk.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 4xkk_SNE_VX_KK)
{
    const uint16_t baseOpcode = 0x4000;
    const uint8_t vxReg = 5;
    const uint8_t targetValue = 0x2A;

    // Test 1: Vx != kk -> skip
    {
		// -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8) | targetValue;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = 0x2B;

		// -- Act --
        ExecuteInstruction();

		// -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

		// -- Reset --
		mInterpreter.Reset();
    }

    // Test 2: Vx == kk -> no skip
    {
		// -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8) | targetValue;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = targetValue;

		// -- Act --
        ExecuteInstruction();

		// -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

        // -- Reset --
        mInterpreter.Reset();
    }
}

// Skip next instruction if Vx = Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 5xy0_SE_VX_VY)
{
    const uint16_t baseOpcode = 0x5000;
    const uint8_t vxReg = 5;
    const uint8_t vyReg = 6;
    const uint8_t matchValue = 0x01;

    // Test 1: Vx == Vy -> skip
    {
		// -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = matchValue;
        GetCPUStateRef().mRegisters[vyReg] = matchValue;

		// -- Act --
        ExecuteInstruction();

		// -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

		// -- Reset --
        mInterpreter.Reset();
    }

    // Test 2: Vx != Vy -> no skip
    {
		// -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = 0x01;
        GetCPUStateRef().mRegisters[vyReg] = 0x02;

		// -- Act --
        ExecuteInstruction();

		// -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

        // -- Reset --
        mInterpreter.Reset();
    }
}

// The interpreter puts the value kk into register Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 6xkk_LD_VX_KK)
{
    // -- Arrange --
    const uint8_t vxReg = 2;
    const uint8_t kkValue = 0x06;
    const uint16_t opcode = 0x6000 | (vxReg << 8) | kkValue;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(kkValue, GetCPUStateRef().mRegisters[vxReg]);
}

// Set Vx = Vx + kk.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 7xkk_ADD_VX_KK)
{
    // -- Arrange --
    const uint8_t vxReg = 6;
    const uint8_t kkValue = 0xFE;
    const uint8_t initialValue = 1;
    const uint16_t opcode = 0x7000 | (vxReg << 8) | kkValue;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mRegisters[vxReg] = initialValue;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    const uint8_t expectedValue = initialValue + kkValue;

    ASSERT_EQ(expectedValue, GetCPUStateRef().mRegisters[vxReg]);
}

// Set Vx = Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy0_LD_VX_VY)
{
    // -- Arrange --
    const uint8_t vxReg = 7;
    const uint8_t vyReg = 5;
    const uint8_t vyValue = 8;
    const uint16_t opcode = 0x8000 | (vxReg << 8) | (vyReg << 4);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mRegisters[vyReg] = vyValue;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(vyValue, GetCPUStateRef().mRegisters[vxReg]);
}

// Set Vx = Vx OR Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy1_OR_VX_VY)
{
    // -- Arrange --
    const uint8_t vxReg = 7;
    const uint8_t vyReg = 3;
    const uint8_t vxValue = 0b00001111;
    const uint8_t vyValue = 0b11110000;
    const uint16_t opcode = 0x8001 | (vxReg << 8) | (vyReg << 4);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mRegisters[vxReg] = vxValue;
    GetCPUStateRef().mRegisters[vyReg] = vyValue;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    const uint8_t expectedValue = vxValue | vyValue;
    
    ASSERT_EQ(expectedValue, GetCPUStateRef().mRegisters[vxReg]);
}

// Set Vx = Vx AND Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy2_AND_VX_VY)
{
    // -- Arrange --
    const uint8_t vxReg = 7;
    const uint8_t vyReg = 3;
    const uint8_t vxValue = 0b01011010;
    const uint8_t vyValue = 0b00111100;
    const uint16_t opcode = 0x8002 | (vxReg << 8) | (vyReg << 4);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mRegisters[vxReg] = vxValue;
    GetCPUStateRef().mRegisters[vyReg] = vyValue;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    const uint8_t expectedValue = vxValue & vyValue;
    
    ASSERT_EQ(expectedValue, GetCPUStateRef().mRegisters[vxReg]);
}

// Set Vx = Vx XOR Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy3_XOR_VX_VY)
{
    // -- Arrange --
    const uint8_t vxReg = 7;
    const uint8_t vyReg = 3;
    const uint8_t vxValue = 0b01011010;
    const uint8_t vyValue = 0b00111100;    
    const uint16_t opcode = 0x8003 | (vxReg << 8) | (vyReg << 4);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mRegisters[vxReg] = vxValue;
    GetCPUStateRef().mRegisters[vyReg] = vyValue;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    const uint8_t expectedValue = vxValue ^ vyValue;
    
    ASSERT_EQ(expectedValue, GetCPUStateRef().mRegisters[vxReg]);
}

// Set Vx = Vx + Vy, set VF = carry.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy4_ADD_VX_VY)
{    
    /*
        NOTE: Test 4 ensures flag is preserved when x == VF.
        (See errata: https://github.com/gulrak/cadmium/wiki/CTR-Errata#math-opcodes)
    */

    const uint16_t baseOpcode = 0x8004;
    const uint8_t vxReg = 7;
    const uint8_t vyReg = 6;
    const uint8_t carry = 1;
    const uint8_t noCarry = 0;

    // Test 1: Vx + Vy > 255 -> carry
    {
        // -- Arrange --
        const uint8_t vxValue = 200;
        const uint8_t vyValue = 100;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = vxValue;
        GetCPUStateRef().mRegisters[vyReg] = vyValue;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(static_cast<uint8_t>(vxValue + vyValue), GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(carry, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 2: Vx + Vy == 255 -> no carry
    {
        // -- Arrange --
        const uint8_t vxValue = 127;
        const uint8_t vyValue = 128;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = vxValue;
        GetCPUStateRef().mRegisters[vyReg] = vyValue;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(static_cast<uint8_t>(vxValue + vyValue), GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(noCarry, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 3: Vx + Vy < 255 -> no carry
    {
        // -- Arrange --
        const uint8_t vxValue = 16;
        const uint8_t vyValue = 32;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = vxValue;
        GetCPUStateRef().mRegisters[vyReg] = vyValue;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(static_cast<uint8_t>(vxValue + vyValue), GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(noCarry, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 4: x == VF -> store flag, not result
    {
        // -- Arrange --
        const uint8_t vxRegTest4 = 0xF;
        const uint8_t vyRegTest4 = 1;
        const uint8_t vxValue = 200;
        const uint8_t vyValue = 100;
        const uint16_t opcode = baseOpcode | (vxRegTest4 << 8) | (vyRegTest4 << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxRegTest4] = vxValue;
        GetCPUStateRef().mRegisters[vyRegTest4] = vyValue;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(carry, GetCPUStateRef().mRegisters[0xF]) << "VF should store carry flag, not arithmetic result.";

        // -- Reset --
        mInterpreter.Reset();
    }
}

// Set Vx = Vx - Vy, set VF = NOT borrow.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy5_SUB_VX_VY)
{
    /*
		 NOTE: 
          - Only borrow flag is set when Vx < Vy.
          - Test 4 ensures flag is preserved when x == VF.
         (See errata: https://github.com/gulrak/cadmium/wiki/CTR-Errata#math-opcodes)
    */

    const uint16_t baseOpcode = 0x8005;
    const uint8_t vxReg = 7;
    const uint8_t vyReg = 6;
    const uint8_t noBorrow = 1;
    const uint8_t borrow = 0;

    // Test 1: Vx > Vy -> no borrow
    {
        // -- Arrange --
        const uint8_t vxValue = 75;
        const uint8_t vyValue = 45;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = vxValue;
        GetCPUStateRef().mRegisters[vyReg] = vyValue;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(vxValue - vyValue, GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(noBorrow, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 2: Vx == Vy -> no borrow
    {
        // -- Arrange --
        const uint8_t value = 42;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = value;
        GetCPUStateRef().mRegisters[vyReg] = value;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(0, GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(noBorrow, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 3: Vx < Vy -> borrow
    {
        // -- Arrange --
        const uint8_t vxValue = 16;
        const uint8_t vyValue = 32;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = vxValue;
        GetCPUStateRef().mRegisters[vyReg] = vyValue;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
		const uint8_t wrappedValue = static_cast<uint8_t>(vxValue - vyValue);

        ASSERT_EQ(wrappedValue, GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(borrow, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 4: x == VF -> store flag, not result
    {
        // -- Arrange --
        const uint8_t vxRegTest4 = 0xF;
        const uint8_t vyRegTest4 = 1;
        const uint8_t vxValue = 100;
        const uint8_t vyValue = 50;
        const uint16_t opcode = baseOpcode | (vxRegTest4 << 8) | (vyRegTest4 << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxRegTest4] = vxValue;
        GetCPUStateRef().mRegisters[vyRegTest4] = vyValue;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(noBorrow, GetCPUStateRef().mRegisters[0xF]) << "VF should contain the no-borrow flag, not the result.";

        // -- Reset --
        mInterpreter.Reset();
    }
}

// Set Vx = Vx SHR 1.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy6_SHR_VX_VY)
{
    /*
        NOTE: In original CHIP-8, Vy is ignored for 8xy6.
        Vx is both the input and output register.

        Test 3 ensures flag is preserved when x == VF.
        (See errata: https://github.com/gulrak/cadmium/wiki/CTR-Errata#math-opcodes)
    */

    const uint16_t baseOpcode = 0x8006;
    const uint8_t vxReg = 6;
    const uint8_t vyReg = 6;
    const uint8_t lsb1 = 1;
    const uint8_t lsb0 = 0;

    // Test 1: LSB of Vx is 1 -> VF = 1
    {
        // -- Arrange --
        const uint8_t value = 0b00111111;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = value;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(value >> 1, GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(lsb1, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 2: LSB of Vx is 0 -> VF = 0
    {
        // -- Arrange --
        const uint8_t value = 0b00111110;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = value;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(value >> 1, GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(lsb0, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 3: x == VF -> store flag, not result
    {
        // -- Arrange --
        const uint8_t vxRegTest3 = 0xF;
        const uint8_t vyRegTest3 = 1;
        const uint8_t value = 0b00000001;
        const uint16_t opcode = baseOpcode | (vxRegTest3 << 8) | (vyRegTest3 << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxRegTest3] = value;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(lsb1, GetCPUStateRef().mRegisters[0xF]) << "VF should store LSB flag, not result value.";

        // -- Reset --
        mInterpreter.Reset();
    }
}

// Set Vx = Vy - Vx, set VF = NOT borrow.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy7_SUBN_VX_VY)
{
    /*
         NOTE:
          - Only borrow flag is set when Vy < Vx.
          - Test 4 ensures flag is preserved when x == VF.
         (See errata: https://github.com/gulrak/cadmium/wiki/CTR-Errata#math-opcodes)
    */

    const uint16_t baseOpcode = 0x8007;
    const uint8_t vxReg = 7;
    const uint8_t vyReg = 6;
    const uint8_t noBorrow = 1;
    const uint8_t borrow = 0;

    // Test 1: Vy > Vx -> no borrow
    {
        // -- Arrange --
        const uint8_t vxValue = 45;
        const uint8_t vyValue = 75;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = vxValue;
        GetCPUStateRef().mRegisters[vyReg] = vyValue;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(vyValue - vxValue, GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(noBorrow, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 2: Vy == Vx -> no borrow
    {
        // -- Arrange --
        const uint8_t value = 42;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = value;
        GetCPUStateRef().mRegisters[vyReg] = value;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(0, GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(noBorrow, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 3: Vy < Vx -> borrow
    {
        // -- Arrange --
        const uint8_t vxValue = 32;
        const uint8_t vyValue = 16;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = vxValue;
        GetCPUStateRef().mRegisters[vyReg] = vyValue;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        const uint8_t wrappedValue = static_cast<uint8_t>(vyValue - vxValue);

        ASSERT_EQ(wrappedValue, GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(borrow, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 4: x == VF -> store flag, not result
    {
        // -- Arrange --
        const uint8_t vxRegTest4 = 0xF;
        const uint8_t vyRegTest4 = 1;
        const uint8_t vxValue = 50;
        const uint8_t vyValue = 100;
        const uint16_t opcode = baseOpcode | (vxRegTest4 << 8) | (vyRegTest4 << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxRegTest4] = vxValue;
        GetCPUStateRef().mRegisters[vyRegTest4] = vyValue;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(noBorrow, GetCPUStateRef().mRegisters[0xF]) << "VF should contain the no-borrow flag, not the result.";

        // -- Reset --
        mInterpreter.Reset();
    }
}

// Set Vx = Vx SHL 1.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xyE_SHL_VX_VY)
{
    /*
        NOTE: In original CHIP-8, Vy is ignored for 8xyE.
        Vx is both the input and output register.

        Test 3 ensures flag is preserved when x == VF.
        (See errata: https://github.com/gulrak/cadmium/wiki/CTR-Errata#math-opcodes)
    */

    const uint16_t baseOpcode = 0x800E;
    const uint8_t vxReg = 6;
    const uint8_t vyReg = 6;
    const uint8_t msb1 = 1;
    const uint8_t msb0 = 0;

    // Test 1: MSB of Vx is 1 -> VF = 1
    {
        // -- Arrange --
        const uint8_t value = 0b10111111;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = value;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(static_cast<uint8_t>(value << 1), GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(msb1, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 2: MSB of Vx is 0 -> VF = 0
    {
        // -- Arrange --
        const uint8_t value = 0b00111110;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = value;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(static_cast<uint8_t>(value << 1), GetCPUStateRef().mRegisters[vxReg]);
        ASSERT_EQ(msb0, GetCPUStateRef().mRegisters[0xF]);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 3: x == VF -> store flag, not result
    {
        // -- Arrange --
        const uint8_t vxRegTest3 = 0xF; // overrides vxReg to test x == VF
        const uint8_t vyRegTest3 = 1;
        const uint8_t value = 0b10000000;
        const uint16_t opcode = baseOpcode | (vxRegTest3 << 8) | (vyRegTest3 << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxRegTest3] = value;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(msb1, GetCPUStateRef().mRegisters[0xF]) << "VF should store MSB flag, not result value.";

        // -- Reset --
        mInterpreter.Reset();
    }
}

// Skip next instruction if Vx != Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 9xy0_SNE_VX_VY)
{
    const uint16_t baseOpcode = 0x9000;
    const uint8_t vxReg = 5;
    const uint8_t vyReg = 6;

    // Test 1: Vx != Vy -> skip
    {
        // -- Arrange --
        const uint8_t vxValue = 0x04;
        const uint8_t vyValue = 0x05;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = vxValue;
        GetCPUStateRef().mRegisters[vyReg] = vyValue;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 2: Vx == Vy -> no skip
    {
        // -- Arrange --
        const uint8_t value = 0x04;
        const uint16_t opcode = baseOpcode | (vxReg << 8) | (vyReg << 4);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = value;
        GetCPUStateRef().mRegisters[vyReg] = value;

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

        // -- Reset --
        mInterpreter.Reset();
    }
}

// The value of register I is set to nnn.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Annn_LD_I_ADDR)
{
    // -- Arrange --
    const uint16_t address = 0x0215;
    const uint16_t opcode = 0xA000 | address;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(address, GetCPUStateRef().mIndexRegister);
}

// Jump to location nnn + V0.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Bnnn_JP_V0_ADDR)
{
    // -- Arrange --
    const uint8_t v0Value = 0x02;
    const uint16_t baseAddress = 0x0200;
    const uint16_t opcode = 0xB000 | baseAddress;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mRegisters[0] = v0Value;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    const uint16_t expectedAddress = baseAddress + v0Value;

    ASSERT_EQ(expectedAddress, GetCPUStateRef().mProgramCounter);
}

// Set Vx = random byte AND kk.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Cxkk_RND_VX_KK)
{
    // -- Arrange --
    const uint8_t vxReg = 2;
    const uint8_t kkValue = 0x33;
    const uint8_t randomByte = 0xAB;

    const uint16_t opcode = 0xC000 | (vxReg << 8) | kkValue;

    EXPECT_CALL(mRandomProvider, GetRandomByte())
        .Times(1)
        .WillOnce(::testing::Return(randomByte));

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    const uint8_t expectedValue = randomByte & kkValue;
    
    ASSERT_EQ(expectedValue, GetCPUStateRef().mRegisters[vxReg]);
}

// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Dxyn_DRW_VX_VY_N)
{
    /*
         NOTE: Only the start position wraps; pixels beyond the screen are clipped.
         (See errata: https://github.com/gulrak/cadmium/wiki/CTR-Errata#dxyn)
    */

    const uint8_t vxReg = 0;
    const uint8_t vyReg = 1;
    DisplayTestAccessor display{ GetBusRef().mDisplay };

    // Test 1: No overlapping pixels -> no collision
    {
        // -- Arrange --
        const uint8_t xPos = 0;
        const uint8_t yPos = 0;
        const uint8_t spriteHeight = 2;
        const uint16_t spriteAddress = 0x202;
        const uint16_t opcode = 0xD000 | (vxReg << 8) | (vyReg << 4) | spriteHeight;

        const uint8_t spriteData[spriteHeight] = {
            0b11111111,
            0b11111101,
        };

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mIndexRegister = spriteAddress;
        GetCPUStateRef().mRegisters[vxReg] = xPos;
        GetCPUStateRef().mRegisters[vyReg] = yPos;

        for (uint8_t i = 0; i < spriteHeight; ++i)
            WriteByteToMemory(spriteAddress + i, spriteData[i]);

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(0, GetCPUStateRef().mRegisters[0xF]);
        ASSERT_TRUE(display.IsPixelSet(7, 1));
        ASSERT_FALSE(display.IsPixelSet(6, 1));

        // -- Reset --
        mInterpreter.Reset();
        display.Clear();
    }

    // Test 2: Overlapping pixels -> collision
    {
        // -- Arrange --
        const uint8_t xPos = 0;
        const uint8_t yPos = 0;
        const uint8_t spriteHeight = 1;
        const uint16_t spriteAddress = 0x210;
        const uint16_t opcode = 0xD000 | (vxReg << 8) | (vyReg << 4) | spriteHeight;

        const uint8_t spriteByte = 0b11111111;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mIndexRegister = spriteAddress;
        GetCPUStateRef().mRegisters[vxReg] = xPos;
        GetCPUStateRef().mRegisters[vyReg] = yPos;

        WriteByteToMemory(spriteAddress, spriteByte);
        ExecuteInstruction(); // First draw, no collision

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        ExecuteInstruction(); // Second draw, collision expected

        // -- Assert --
        ASSERT_EQ(1, GetCPUStateRef().mRegisters[0xF]);
        ASSERT_FALSE(display.IsPixelSet(0, 0));

        // -- Reset --
        mInterpreter.Reset();
        display.Clear();
    }
    
    // Test 3: Sprite clips when pixel exceeds display bounds
    {
        // -- Arrange --
        const uint8_t visibleWidth = 4;  // Right 4 bits clipped
        const uint8_t visibleHeight = 2; // 2 rows fit, 1 row clipped
        const uint8_t xPos = DISPLAY_WIDTH - visibleWidth;
        const uint8_t yPos = DISPLAY_HEIGHT - visibleHeight;
        const uint8_t spriteHeight = 3;
        const uint16_t spriteAddress = 0x220;
        const uint16_t opcode = 0xD000 | (vxReg << 8) | (vyReg << 4) | spriteHeight;

        const uint8_t spriteData[spriteHeight] = {
            0b11111111, // row 0: partially visible
            0b11111111, // row 1: partially visible
            0b11111111  // row 2: completely clipped vertically
        };

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mIndexRegister = spriteAddress;
        GetCPUStateRef().mRegisters[vxReg] = xPos;
        GetCPUStateRef().mRegisters[vyReg] = yPos;

        for (uint8_t i = 0; i < spriteHeight; ++i)
        {
            WriteByteToMemory(spriteAddress + i, spriteData[i]);
        }

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        for (uint8_t row = 0; row < spriteHeight; ++row)
        {
            for (uint8_t col = 0; col < 8; ++col)
            {
                const bool withinVisibleRegion = (col < visibleWidth) && (row < visibleHeight);
                const uint8_t screenX = (xPos + col) % DISPLAY_WIDTH;
                const uint8_t screenY = (yPos + row) % DISPLAY_HEIGHT;

                ASSERT_EQ(withinVisibleRegion, display.IsPixelSet(screenX, screenY))
                    << "Expected pixel at (" << static_cast<int>(screenX)
                    << "," << static_cast<int>(screenY)
                    << ") to be " << (withinVisibleRegion ? "set" : "clear");
            }
        }

        // -- Reset --
        mInterpreter.Reset();
        display.Clear();
    }

    // Test 4: Sprite wraps around when X or Y position exceeds display bounds
    {
        // -- Arrange --
        const uint8_t xWrapInput = DISPLAY_WIDTH + 2;  // Will wrap to 2
        const uint8_t yWrapInput = DISPLAY_HEIGHT + 1; // Will wrap to 1
        const uint8_t spriteHeight = 1;

        const uint16_t spriteAddress = 0x230;
        const uint8_t spriteByte = 0b11000000; // Only first two bits set
        const uint16_t opcode = 0xD000 | (vxReg << 8) | (vyReg << 4) | spriteHeight;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mIndexRegister = spriteAddress;
        GetCPUStateRef().mRegisters[vxReg] = xWrapInput;
        GetCPUStateRef().mRegisters[vyReg] = yWrapInput;

        WriteByteToMemory(spriteAddress, spriteByte);

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        const uint8_t expectedX = xWrapInput % DISPLAY_WIDTH;  // 2
        const uint8_t expectedY = yWrapInput % DISPLAY_HEIGHT; // 1

        ASSERT_TRUE(display.IsPixelSet(expectedX, expectedY)) << "Expected pixel at (2,1) to be set";
        ASSERT_TRUE(display.IsPixelSet(expectedX + 1, expectedY)) << "Expected pixel at (3,1) to be set";

        // Pixels beyond those two should remain unset
        ASSERT_FALSE(display.IsPixelSet((expectedX + 2) % DISPLAY_WIDTH, expectedY))
            << "Unexpected pixel set at x=" << (expectedX + 2) % DISPLAY_WIDTH;

        // -- Reset --
        mInterpreter.Reset();
        display.Clear();
    }
}

// Skip next instruction if key with the value of Vx is pressed.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Ex9E_SKP_VX)
{
    const uint16_t baseOpcode = 0xE09E;
    const uint8_t vxReg = 1;
    const Key key{ Key::Key5 };

    // Test 1: Key is pressed -> skip
    {
        // -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = key.GetValue();
        GetBusRef().mKeypad.SetKeyPressed(key, true);

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 2: Key is not pressed -> no skip
    {
        // -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = key.GetValue();
        GetBusRef().mKeypad.SetKeyPressed(key, false);

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

        // -- Reset --
        mInterpreter.Reset();
    }
}

// Skip next instruction if key with the value of Vx is not pressed.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, ExA1_SKNP_VX)
{
    const uint16_t baseOpcode = 0xE0A1;
    const uint8_t vxReg = 1;
    const Key key{ Key::Key5 };	

    // Test 1: Key is not pressed -> skip
    {
        // -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = key.GetValue();
        GetBusRef().mKeypad.SetKeyPressed(key, false);

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

        // -- Reset --
        mInterpreter.Reset();
    }

    // Test 2: Key is pressed -> no skip
    {
        // -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = key.GetValue();
        GetBusRef().mKeypad.SetKeyPressed(key, true);

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);

        // -- Reset --
        mInterpreter.Reset();
    }
}

// Set Vx = delay timer value.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx07_LD_VX_DT)
{
    // -- Arrange --
    const uint8_t vxReg = 4;
    const uint8_t timerValue = 120;
    const uint16_t opcode = 0xF007 | (vxReg << 8);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mDelayTimer = timerValue;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(timerValue, GetCPUStateRef().mRegisters[vxReg]);
}

// Wait for a key release, store the value of the key in Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx0A_LD_VX_K)
{
    /*
         NOTE: Fx0A waits for a key release, not a key press.
         (See errata: https://github.com/gulrak/cadmium/wiki/CTR-Errata#fx0a)
    */

    // -- Assert --
    const uint8_t vxReg = 0;
    const Key key{ Key::Key2 };
    const uint16_t opcode = 0xF00A | (vxReg << 8);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);

    // -- Act --: execute without key press
	ExecuteInstruction(ExecutionStatus::WaitingOnKeyPress);

    // -- Assert --: no change to register or PC
    EXPECT_EQ(GetCPUStateRef().mRegisters[vxReg], 0x00);
    EXPECT_EQ(GetCPUStateRef().mProgramCounter, PROGRAM_START_ADDRESS);

    // -- Assert --: simulate key release
    GetBusRef().mKeypad.SetKeyPressed(key, true);
    GetBusRef().mKeypad.SetKeyPressed(key, false);

    // -- Act --: execute again with key press
    ExecuteInstruction(ExecutionStatus::Executed);

    // -- Assert --: Vx set, PC advanced
    EXPECT_EQ(GetCPUStateRef().mRegisters[vxReg], key.GetValue());
    EXPECT_EQ(GetCPUStateRef().mProgramCounter, PROGRAM_START_ADDRESS + INSTRUCTION_SIZE);
}

// Set delay timer = Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx15_LD_DT_VX)
{
    // -- Arrange --
    const uint8_t vxReg = 3;
    const uint8_t vxValue = 120;
    const uint16_t opcode = 0xF015 | (vxReg << 8);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mRegisters[vxReg] = vxValue;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(vxValue, GetCPUStateRef().mDelayTimer);
}

// Set sound timer = Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx18_LD_ST_VX)
{
    // -- Arrange --
    const uint8_t vxReg = 3;
    const uint8_t vxValue = 120;
    const uint16_t opcode = 0xF018 | (vxReg << 8);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mRegisters[vxReg] = vxValue;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(vxValue, GetCPUStateRef().mSoundTimer);
}

// Set I = I + Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx1E_ADD_I_VX)
{
    // -- Arrange --
    const uint8_t vxReg = 6;
    const uint8_t vxValue = 0x20;
    const uint16_t initialIndex = 0x0100;
    const uint16_t opcode = 0xF01E | (vxReg << 8);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mIndexRegister = initialIndex;
    GetCPUStateRef().mRegisters[vxReg] = vxValue;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    const uint16_t expectedIndex = initialIndex + vxValue;

    ASSERT_EQ(expectedIndex, GetCPUStateRef().mIndexRegister);
}

// Set I = location of sprite for digit Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx29_LD_F_VX)
{
    // -- Assert --
    const uint8_t vxReg = 8;
	const uint8_t digit = 0xA; // Hexadecimal digit A (see Constants.h for font sprites)
    const uint16_t opcode = 0xF029 | (vxReg << 8);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
	GetCPUStateRef().mRegisters[vxReg] = digit;

    // -- Act --
    ExecuteInstruction();

	// -- Assert --    
    EXPECT_EQ(GetCPUStateRef().mIndexRegister, digit * kFontSpriteSize);
}

// Store BCD representation of Vx in memory locations I, I+1, and I+2.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx33_LD_B_VX)
{
    // -- Arrange --
    const uint8_t vxReg = 6;
    const uint8_t vxValue = 123; // BCD: 1, 2, 3
    const uint16_t memoryAddress = PROGRAM_START_ADDRESS + 20;
    const uint16_t opcode = 0xF033 | (vxReg << 8);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mIndexRegister = memoryAddress;
    GetCPUStateRef().mRegisters[vxReg] = vxValue;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(1, ReadByteFromMemory(memoryAddress + 0)); // Hundreds
    ASSERT_EQ(2, ReadByteFromMemory(memoryAddress + 1)); // Tens
    ASSERT_EQ(3, ReadByteFromMemory(memoryAddress + 2)); // Units
}

// Store registers V0 through Vx in memory starting at location I.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx55_LD_I_VX)
{
    // -- Arrange --
    const uint8_t vxReg = 6;
    const uint16_t memoryAddress = PROGRAM_START_ADDRESS + 20;
    const uint16_t opcode = 0xF055 | (vxReg << 8);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mIndexRegister = memoryAddress;

    for (uint8_t i = 0; i <= vxReg; ++i)
    {
        GetCPUStateRef().mRegisters[i] = static_cast<uint8_t>(i * 10);
    }

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    for (uint8_t i = 0; i <= vxReg; ++i)
    {
        const uint16_t address = memoryAddress + i;
        const uint8_t expectedValue = GetCPUStateRef().mRegisters[i];
        ASSERT_EQ(expectedValue, ReadByteFromMemory(address));
    }
}

// Read registers V0 through Vx from memory starting at location I.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx65_LD_VX_I)
{
    // -- Arrange --
    const uint8_t vxReg = 6;
    const uint16_t memoryAddress = PROGRAM_START_ADDRESS + 20;
    const uint16_t opcode = 0xF065 | (vxReg << 8);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
    GetCPUStateRef().mIndexRegister = memoryAddress;

    for (uint8_t i = 0; i <= vxReg; ++i)
    {
        WriteByteToMemory(memoryAddress + i, static_cast<uint8_t>(i * 10));
    }

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    for (uint8_t i = 0; i <= vxReg; ++i)
    {
        const uint8_t expectedValue = ReadByteFromMemory(memoryAddress + i);
        ASSERT_EQ(expectedValue, GetCPUStateRef().mRegisters[i]);
    }
}