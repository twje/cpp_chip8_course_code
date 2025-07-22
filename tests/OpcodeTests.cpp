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
	// -- Assert --
	WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x00E0);    
    
    Display& display = GetBusRef().mDisplay;
	display.SetPixel(0, 0, true);

	// -- Act --
	ExecuteInstruction();

	// -- Assert --	
	ASSERT_FALSE(display.IsPixelSet(0, 0));
}

// Return from a subroutine.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 00EE_RET)
{    
    // -- Assert --    
    const uint16_t returnAdress = PROGRAM_START_ADDRESS + 10;
 
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x00EE);    
    GetCPUStateRef().mStack[0] = returnAdress;
    GetCPUStateRef().mStackPointer = 1;

    // -- Act --
	ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(0, GetCPUStateRef().mStackPointer);
    ASSERT_EQ(returnAdress, GetCPUStateRef().mProgramCounter);    
}

// Jump to location nnn.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 1nnn_JP_ADDR)
{
    // -- Assert --
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x124E);

    // -- Act --
	const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(0x024E, GetCPUStateRef().mProgramCounter);
    const uint16_t address = instruction.GetOperandNNN();
    ASSERT_EQ(0x024E, address);
}

// Call subroutine at nnn
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 2nnn_CALL_ADDR)
{
    // -- Assert --
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x2F23);    

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mStack[0]);
    ASSERT_EQ(1, GetCPUStateRef().mStackPointer);
    ASSERT_EQ(0x0F23, GetCPUStateRef().mProgramCounter);
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
    // -- Assert --
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x6206);

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();
    
    // -- Assert --
	const size_t index = instruction.GetOperandX();
    const uint8_t value = instruction.GetOperandKK();

    ASSERT_EQ(GetCPUStateRef().mRegisters[index], value);
}

// Set Vx = Vx + kk.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 7xkk_ADD_VX_KK)
{
    // -- Assert --
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x76FE);
    GetCPUStateRef().mRegisters[6] = 1;

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    const size_t index = instruction.GetOperandX();
    const uint8_t value = instruction.GetOperandKK();

    ASSERT_EQ(6, index);
    ASSERT_EQ(254, value);
    ASSERT_EQ(GetCPUStateRef().mRegisters[index], 255);
}

// Set Vx = Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy0_LD_VX_VY)
{
    // -- Assert --
    const uint8_t vyValue = 8;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8750);    
    GetCPUStateRef().mRegisters[5] = vyValue;

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    const size_t vxIndex = instruction.GetOperandX();
    const size_t vyIndex = instruction.GetOperandY();

    ASSERT_EQ(7, vxIndex);
    ASSERT_EQ(5, vyIndex);
    ASSERT_EQ(vyValue, GetCPUStateRef().mRegisters[vxIndex]);
}

// Set Vx = Vx OR Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy1_OR_VX_VY)
{
    // -- Assert --
    const uint8_t vxValue = 0b00001111;
    const uint8_t vyValue = 0b11110000;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8731);
    GetCPUStateRef().mRegisters[7] = vxValue;
    GetCPUStateRef().mRegisters[3] = vyValue;

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    const size_t vxIndex = instruction.GetOperandX();
    const size_t vyIndex = instruction.GetOperandY();
    const uint8_t result = vxValue | vyValue; // 0b11111111

    ASSERT_EQ(7, vxIndex);
    ASSERT_EQ(3, vyIndex);
    ASSERT_EQ(result, GetCPUStateRef().mRegisters[vxIndex]);
}

// Set Vx = Vx AND Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy2_AND_VX_VY)
{
    // -- Assert --
    const uint8_t vxValue = 0b01011010;
    const uint8_t vyValue = 0b00111100;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8732);
    GetCPUStateRef().mRegisters[7] = vxValue;
    GetCPUStateRef().mRegisters[3] = vyValue;

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    const size_t vxIndex = instruction.GetOperandX();
    const size_t vyIndex = instruction.GetOperandY();
    const uint8_t result = vxValue & vyValue; // 0b00011000
    
    ASSERT_EQ(7, vxIndex);
    ASSERT_EQ(3, vyIndex);
    ASSERT_EQ(result, GetCPUStateRef().mRegisters[vxIndex]);
}

// Set Vx = Vx XOR Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy3_XOR_VX_VY)
{
    // -- Assert --
    const uint8_t vxValue = 0b01011010;
    const uint8_t vyValue = 0b00111100;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8733);
    GetCPUStateRef().mRegisters[7] = vxValue;
    GetCPUStateRef().mRegisters[3] = vyValue;

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    const size_t vxIndex = instruction.GetOperandX();
    const size_t vyIndex = instruction.GetOperandY();
    const uint8_t result = vxValue ^ vyValue; // 0b01100110

    ASSERT_EQ(7, vxIndex);
    ASSERT_EQ(3, vyIndex);
    ASSERT_EQ(result, GetCPUStateRef().mRegisters[vxIndex]);
}

// Set Vx = Vx + Vy, set VF = carry.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy4_ADD_VX_VY)
{    
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
        ASSERT_EQ(static_cast<uint8_t>(vxValue - vyValue), GetCPUStateRef().mRegisters[vxReg]);
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
        ASSERT_EQ(static_cast<uint8_t>(vyValue - vxValue), GetCPUStateRef().mRegisters[vxReg]);
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
    // -- Assert --
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xA215);

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(0x0215, instruction.GetOperandNNN());
    ASSERT_EQ(0x0215, GetCPUStateRef().mIndexRegister);	
}

// Jump to location nnn + V0.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Bnnn_JP_V0_ADDR)
{
    // -- Assert --
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xB202);
    GetCPUStateRef().mRegisters[0] = 0x02;

    // -- Act --
    ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(0x204, GetCPUStateRef().mProgramCounter);
}

// Set Vx = random byte AND kk.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Cxkk_RND_VX_KK)
{
    // -- Assert --
    const uint8_t vxIndex = 2;
    const uint8_t kk = 0x33;    
    
    EXPECT_CALL(mRandomProvider, GetRandomByte())
        .Times(1)
        .WillOnce(::testing::Return(kMockedRandomValue));
    
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xC200 | kk);

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(instruction.GetOperandX(), vxIndex);
    ASSERT_EQ(instruction.GetOperandKK(), kk);
    ASSERT_EQ(GetCPUStateRef().mRegisters[vxIndex], kMockedRandomValue & kk);
}

// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Dxyn_DRW_VX_VY_N)
{
    const uint8_t vxReg = 0;
    const uint8_t vyReg = 1;
    const Display& display = GetBusRef().mDisplay;

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
        GetBusRef().mDisplay.Clear();
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
        GetBusRef().mDisplay.Clear();
    }

    // Test 3: Sprite wraps around screen horizontally
    {
        // -- Arrange --
        const uint8_t xPos = DISPLAY_WIDTH - 4;
        const uint8_t yPos = 0;
        const uint8_t spriteHeight = 1;
        const uint16_t spriteAddress = 0x220;
        const uint16_t opcode = 0xD000 | (vxReg << 8) | (vyReg << 4) | spriteHeight;

        const uint8_t spriteByte = 0b11111111;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mIndexRegister = spriteAddress;
        GetCPUStateRef().mRegisters[vxReg] = xPos;
        GetCPUStateRef().mRegisters[vyReg] = yPos;

        WriteByteToMemory(spriteAddress, spriteByte);

        // -- Act --
        ExecuteInstruction();

        // -- Assert --
        for (uint8_t i = 0; i < 8; ++i)
        {
            const uint8_t screenX = (xPos + i) % DISPLAY_WIDTH;
            ASSERT_TRUE(display.IsPixelSet(screenX, yPos)) << "Pixel not set at X=" << screenX;
        }

        // -- Reset --
        mInterpreter.Reset();
        GetBusRef().mDisplay.Clear();
    }
}

// Skip next instruction if key with the value of Vx is pressed.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Ex9E_SKP_VX)
{
    const uint16_t baseOpcode = 0xE09E;
    const uint8_t vxReg = 1;
    const Keypad::Key key = Keypad::Key::Key5;
    const uint8_t keyValue = Keypad::KeyToIndex(key);

    // Test 1: Key is pressed -> skip
    {
        // -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = keyValue;
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
        GetCPUStateRef().mRegisters[vxReg] = keyValue;
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
    const Keypad::Key key = Keypad::Key::Key5;
    const uint8_t keyValue = Keypad::KeyToIndex(key);

    // Test 1: Key is not pressed -> skip
    {
        // -- Arrange --
        const uint16_t opcode = baseOpcode | (vxReg << 8);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
        GetCPUStateRef().mRegisters[vxReg] = keyValue;
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
        GetCPUStateRef().mRegisters[vxReg] = keyValue;
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
    // -- Assert --
    const uint8_t value = 120;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF407);
    GetCPUStateRef().mDelayTimer = value;

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    const size_t index = instruction.GetOperandX();

    ASSERT_EQ(4, index);
    ASSERT_EQ(value, GetCPUStateRef().mRegisters[index]);
}

// Wait for a key press, store the value of the key in Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx0A_LD_VX_K)
{
    // -- Assert --
    const uint8_t vxRegister = 0;
    const auto key = Keypad::Key::Key2;

    const uint16_t opcode =
        0xF00A |
        (vxRegister << 8);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);

    // -- Act --: execute without key press
	ExecuteInstruction(ExecutionStatus::WaitingOnKeyPress);

    // -- Assert --: no change to register or PC
    EXPECT_EQ(GetCPUStateRef().mRegisters[vxRegister], 0x00);
    EXPECT_EQ(GetCPUStateRef().mProgramCounter, PROGRAM_START_ADDRESS);

    // -- Assert --: simulate key press
    GetBusRef().mKeypad.SetKeyPressed(key, true);

    // -- Act --: execute again with key press
    ExecuteInstruction(ExecutionStatus::Executed);

    // -- Assert --: Vx set, PC advanced
    EXPECT_EQ(GetCPUStateRef().mRegisters[vxRegister], Keypad::KeyToIndex(key));
    EXPECT_EQ(GetCPUStateRef().mProgramCounter, PROGRAM_START_ADDRESS + INSTRUCTION_SIZE);
}

// Set delay timer = Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx15_LD_DT_VX)
{
    // -- Assert --
    const uint8_t value = 120;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF315);
    GetCPUStateRef().mRegisters[3] = value;

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(3, instruction.GetOperandX());
    ASSERT_EQ(value, GetCPUStateRef().mDelayTimer);
}

// Set sound timer = Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx18_LD_ST_VX)
{
    // -- Assert --
    const uint8_t value = 120;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF318);
    GetCPUStateRef().mRegisters[3] = value;

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    ASSERT_EQ(3, instruction.GetOperandX());
    ASSERT_EQ(value, GetCPUStateRef().mSoundTimer);
}

// Set I = I + Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx1E_ADD_I_VX)
{
	// -- Assert --
	const uint16_t initialIndex = 0x100;
	const uint8_t value = 0x20; // Vx value to add

	WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF61E);
	GetCPUStateRef().mIndexRegister = initialIndex;
	GetCPUStateRef().mRegisters[6] = value;

	// -- Act --
	const Instruction& instruction = ExecuteInstruction();

	// -- Assert --
	ASSERT_EQ(6, instruction.GetOperandX());
	ASSERT_EQ(initialIndex + value, GetCPUStateRef().mIndexRegister);
}

// Set I = location of sprite for digit Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx29_LD_F_VX)
{
    // -- Assert --
    const uint8_t vxRegister = 8;
	const uint8_t digit = 0xA; // Hexadecimal digit A (see Constants.h for font sprites)

    const uint16_t opcode =
        0xF029 |
        (vxRegister << 8);

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, opcode);
	GetCPUStateRef().mRegisters[vxRegister] = digit;

    // -- Act --
    ExecuteInstruction();

	// -- Assert --    
    EXPECT_EQ(GetCPUStateRef().mIndexRegister, digit * kFontSpriteSize);
}

// Store BCD representation of Vx in memory locations I, I+1, and I+2.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx33_LD_B_VX)
{
	// -- Assert --
	const uint8_t value = 123; // BCD representation: 1, 2, 3
	const uint16_t memoryWriteStartAddress = PROGRAM_START_ADDRESS + 20;

	WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF633);
	GetCPUStateRef().mIndexRegister = memoryWriteStartAddress;
	GetCPUStateRef().mRegisters[6] = value;

	// -- Act --
	const Instruction& instruction = ExecuteInstruction();

	// -- Assert --
	ASSERT_EQ(6, instruction.GetOperandX());

	ASSERT_EQ(1, ReadByteFromMemory(memoryWriteStartAddress + 0)); // Hundreds place
	ASSERT_EQ(2, ReadByteFromMemory(memoryWriteStartAddress + 1)); // Tens place
	ASSERT_EQ(3, ReadByteFromMemory(memoryWriteStartAddress + 2)); // Units place
}

// Store registers V0 through Vx in memory starting at location I.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx55_LD_I_VX)
{
    const uint16_t memoryWriteStartAddress = PROGRAM_START_ADDRESS + 20;
    
    // -- Assert --
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF655);
    GetCPUStateRef().mIndexRegister = memoryWriteStartAddress;
    
    for (size_t i = 0; i <= 6; i++)
    {
        GetCPUStateRef().mRegisters[i] = static_cast<uint8_t>(i * 10);
    }

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    const size_t lastRegisterIndex = instruction.GetOperandX();
    ASSERT_EQ(6, lastRegisterIndex);

    for (uint8_t i = 0; i <= lastRegisterIndex; i++)
    {
        const uint16_t address = memoryWriteStartAddress + i;
        const uint8_t expected = GetCPUStateRef().mRegisters[i];
        ASSERT_EQ(expected, ReadByteFromMemory(address));
    }
}

// Read registers V0 through Vx from memory starting at location I.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx65_LD_VX_I)
{
    const uint16_t memoryReadStartAddress = PROGRAM_START_ADDRESS + 20;

    // -- Assert --
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF665);
    GetCPUStateRef().mIndexRegister = memoryReadStartAddress;

    for (uint16_t i = 0; i <= 6; i++)
    {
		WriteByteToMemory(memoryReadStartAddress + i, static_cast<uint8_t>(i * 10));        
    }

    // -- Act --
    const Instruction& instruction = ExecuteInstruction();

    // -- Assert --
    const size_t lastRegisterIndex = instruction.GetOperandX();
    ASSERT_EQ(6, lastRegisterIndex);

    for (uint8_t i = 0; i <= lastRegisterIndex; i++)
    {
        const uint16_t address = memoryReadStartAddress + i;
        const uint8_t expected = ReadByteFromMemory(address);
        ASSERT_EQ(expected, GetCPUStateRef().mRegisters[i]);
    }
}
