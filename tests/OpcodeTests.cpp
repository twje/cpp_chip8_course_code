#define UNIT_TESTING

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interfaces/IRandomProvider.h"
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
        CPU& cpu = mInterpreter.mCPU;
		
        // Fetch
        const FetchResult fetchResult = mInterpreter.mCPU.Fetch();
        EXPECT_TRUE(fetchResult.mIsValidAddress) << "Fetch failed";

        // Decode
        Instruction instruction = cpu.Decode(fetchResult.mOpcode);
        EXPECT_TRUE(instruction.IsValid()) << "Decode failed";
        
		// Execute
        EXPECT_TRUE(mInterpreter.Step().mStatus == ExecutionStatus::Executed) << "Execution failed";

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
	// Arrange
	WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x00E0);    
    
    Display& display = GetBusRef().mDisplay;
	display.SetPixel(0, 0, true);

	// Act
	ExecuteInstruction();

	// Assert	
	ASSERT_FALSE(display.IsPixelSet(0, 0));
}

// Return from a subroutine.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 00EE_RET)
{    
    // Arrange    
    const uint16_t returnAdress = PROGRAM_START_ADDRESS + 10;
 
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x00EE);    
    GetCPUStateRef().mStack[0] = returnAdress;
    GetCPUStateRef().mStackPointer = 1;

    // Act
	ExecuteInstruction();

    // Assert
    ASSERT_EQ(0, GetCPUStateRef().mStackPointer);
    ASSERT_EQ(returnAdress, GetCPUStateRef().mProgramCounter);    
}

// Jump to location nnn.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 1nnn_JP_ADDR)
{
    // Arrange
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x124E);

    // Act
	const Instruction& instruction = ExecuteInstruction();

    // Assert
    ASSERT_EQ(0x024E, GetCPUStateRef().mProgramCounter);
    const uint16_t address = instruction.GetOperandNNN();
    ASSERT_EQ(0x024E, address);
}

// Call subroutine at nnn
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 2nnn_CALL_ADDR)
{
    // Arrange
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x2F23);    

    // Act
    ExecuteInstruction();

    // Assert
    ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mStack[0]);
    ASSERT_EQ(1, GetCPUStateRef().mStackPointer);
    ASSERT_EQ(0x0F23, GetCPUStateRef().mProgramCounter);
}

// Skip next instruction if Vx = kk.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 3xkk_SE_VX_KK)
{
    // Positive case: skip next instruction
    {
        // Arrange
        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x3205);
        GetCPUStateRef().mRegisters[2] = 0x05;

        // Act
        ExecuteInstruction();
        
        // Assert        
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }

	// Negative case: does not skip next instruction
    {
        // Arrange
        mInterpreter.Reset();
        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x3205);
        GetCPUStateRef().mRegisters[2] = 0x04;

        // Act
        ExecuteInstruction();
        
        // Assert        
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }    
}

// Skip next instruction if Vx != kk.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 4xkk_SNE_VX_KK)
{
    // Positive case: does not skip next instruction
    {
        // Arrange
        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x452A);
        GetCPUStateRef().mRegisters[5] = 0x2B;

        // Act
        ExecuteInstruction();

        // Assert        
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }

    // Negative case: skip next instruction
    {
        // Arrange
        mInterpreter.Reset();
        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x452A);
        GetCPUStateRef().mRegisters[5] = 0x2A;

        // Act
        ExecuteInstruction();

        // Assert        
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }
}

// Skip next instruction if Vx = Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 5xy0_SE_VX_VY)
{
    // Positive case: skip next instruction
    {
        // Arrange
        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x5560);
        GetCPUStateRef().mRegisters[5] = 0x01;
        GetCPUStateRef().mRegisters[6] = 0x01;

        // Act
        ExecuteInstruction();

        // Assert        
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }

    // Negative case: does not skip next instruction
    {
        mInterpreter.Reset();
        
        // Arrange
        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x5560);
        GetCPUStateRef().mRegisters[5] = 0x01;
        GetCPUStateRef().mRegisters[6] = 0x02;

        // Act
        ExecuteInstruction();

        // Assert        
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }
}

// The interpreter puts the value kk into register Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 6xkk_LD_VX_KK)
{
    // Arrange
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x6206);

    // Act
    const Instruction& instruction = ExecuteInstruction();
    
    // Assert
	const size_t index = instruction.GetOperandX();
    const uint8_t value = instruction.GetOperandKK();

    ASSERT_EQ(GetCPUStateRef().mRegisters[index], value);
}

// Set Vx = Vx + kk.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 7xkk_ADD_VX_KK)
{
    // Arrange
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x76FE);
    GetCPUStateRef().mRegisters[6] = 1;

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
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
    // Arrange
    const uint8_t vyValue = 8;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8750);    
    GetCPUStateRef().mRegisters[5] = vyValue;

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
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
    // Arrange
    const uint8_t vxValue = 0b00001111;
    const uint8_t vyValue = 0b11110000;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8731);
    GetCPUStateRef().mRegisters[7] = vxValue;
    GetCPUStateRef().mRegisters[3] = vyValue;

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
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
    // Arrange
    const uint8_t vxValue = 0b01011010;
    const uint8_t vyValue = 0b00111100;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8732);
    GetCPUStateRef().mRegisters[7] = vxValue;
    GetCPUStateRef().mRegisters[3] = vyValue;

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
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
    // Arrange
    const uint8_t vxValue = 0b01011010;
    const uint8_t vyValue = 0b00111100;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8733);
    GetCPUStateRef().mRegisters[7] = vxValue;
    GetCPUStateRef().mRegisters[3] = vyValue;

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
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
    // Test VF = 1 (carry) for Vx + Vy > 255
    {
        // Arrange
        const uint8_t vxValue = 200;
        const uint8_t vyValue = 100;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8924);
        GetCPUStateRef().mRegisters[9] = vxValue;
        GetCPUStateRef().mRegisters[2] = vyValue;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t vxIndex = instruction.GetOperandX();
        const size_t vyIndex = instruction.GetOperandY();
        const uint16_t sum = vxValue + vyValue;

        ASSERT_EQ(9, vxIndex);
        ASSERT_EQ(2, vyIndex);
        ASSERT_EQ(static_cast<uint8_t>(sum & 0xFF), GetCPUStateRef().mRegisters[vxIndex]);
        ASSERT_EQ(1, GetCPUStateRef().mRegisters[0xF]);
    }

    // Test VF = 0 (no carry) for Vx + Vy == 255
    {
        // Arrange
        const uint8_t vxValue = 127;
        const uint8_t vyValue = 128;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8544);
        GetCPUStateRef().mRegisters[5] = vxValue;
        GetCPUStateRef().mRegisters[4] = vyValue;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t vxIndex = instruction.GetOperandX();
        const size_t vyIndex = instruction.GetOperandY();
        const uint16_t sum = vxValue + vyValue;

        ASSERT_EQ(5, vxIndex);
        ASSERT_EQ(4, vyIndex);
        ASSERT_EQ(static_cast<uint8_t>(sum & 0xFF), GetCPUStateRef().mRegisters[vxIndex]);
        ASSERT_EQ(0, GetCPUStateRef().mRegisters[0xF]);
    }

    // Test VF = 0 (no carry) for Vx + Vy < 255
    {
        // Arrange
        const uint8_t vxValue = 16;
        const uint8_t vyValue = 32;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8764);
        GetCPUStateRef().mRegisters[7] = vxValue;
        GetCPUStateRef().mRegisters[6] = vyValue;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t vxIndex = instruction.GetOperandX();
        const size_t vyIndex = instruction.GetOperandY();
        const uint16_t sum = vxValue + vyValue;

        ASSERT_EQ(7, vxIndex);
        ASSERT_EQ(6, vyIndex);
        ASSERT_EQ(static_cast<uint8_t>(sum & 0xFF), GetCPUStateRef().mRegisters[vxIndex]);
        ASSERT_EQ(0, GetCPUStateRef().mRegisters[0xF]);
    }
}

// Set Vx = Vx - Vy, set VF = NOT borrow.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy5_SUB_VX_VY)
{
    // Test VF = 1 (no borrow) for Vx > Vy
    {
        // Arrange
        const uint8_t vxValue = 75;
        const uint8_t vyValue = 45;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8765);
        GetCPUStateRef().mRegisters[7] = vxValue;
        GetCPUStateRef().mRegisters[6] = vyValue;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t vxIndex = instruction.GetOperandX();
        const size_t vyIndex = instruction.GetOperandY();
        const uint8_t subtraction = static_cast<uint8_t>(vxValue - vyValue);

        ASSERT_EQ(7, vxIndex);
        ASSERT_EQ(6, vyIndex);
        ASSERT_EQ(subtraction, GetCPUStateRef().mRegisters[vxIndex]);
        ASSERT_EQ(1, GetCPUStateRef().mRegisters[0xF]);
    }    

    // Test VF = 0 (borrow) for Vx == Vy
    {
        // Arrange
        const uint8_t vxValue = 42;
        const uint8_t vyValue = 42;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8765);
        GetCPUStateRef().mRegisters[7] = vxValue;
        GetCPUStateRef().mRegisters[6] = vyValue;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t vxIndex = instruction.GetOperandX();
        const size_t vyIndex = instruction.GetOperandY();
        const uint8_t subtraction = static_cast<uint8_t>(vxValue - vyValue);

        ASSERT_EQ(7, vxIndex);
        ASSERT_EQ(6, vyIndex);
        ASSERT_EQ(subtraction, GetCPUStateRef().mRegisters[vxIndex]);
        ASSERT_EQ(0, GetCPUStateRef().mRegisters[0xF]);
    }

    // Test VF = 0 (borrow) for Vx < Vy
    {
        // Arrange
        const uint8_t vxValue = 16;
        const uint8_t vyValue = 32;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8765);
        GetCPUStateRef().mRegisters[7] = vxValue;
        GetCPUStateRef().mRegisters[6] = vyValue;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t vxIndex = instruction.GetOperandX();
        const size_t vyIndex = instruction.GetOperandY();
		const uint8_t subtraction = static_cast<uint8_t>(vxValue - vyValue); // Wrapping expected

        ASSERT_EQ(7, vxIndex);
        ASSERT_EQ(6, vyIndex);
        ASSERT_EQ(subtraction, GetCPUStateRef().mRegisters[vxIndex]);
        ASSERT_EQ(0, GetCPUStateRef().mRegisters[0xF]);
    }
}

// Set Vx = Vx SHR 1.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy6_SHR_VX_VY)
{
    // Test VF == 1 (least-significant bit is 1)
    {
        // Arrange
        const uint8_t value = 0b00111111;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8666);
        GetCPUStateRef().mRegisters[6] = value;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t index = instruction.GetOperandX();  // Operand Y is ignored    

        ASSERT_EQ(static_cast<uint8_t>(value / 2), GetCPUStateRef().mRegisters[index]);
        ASSERT_EQ(1, GetCPUStateRef().mRegisters[0xF]);
    }

    // Test VF == 0 (least-significant bit is 0)
    {
        // Arrange
        const uint8_t value = 0b00111110;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8666);
        GetCPUStateRef().mRegisters[6] = value;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t index = instruction.GetOperandX();  // Operand Y is ignored    

        ASSERT_EQ(static_cast<uint8_t>(value / 2), GetCPUStateRef().mRegisters[index]);
        ASSERT_EQ(0, GetCPUStateRef().mRegisters[0xF]);
    }
}

// Set Vx = Vy - Vx, set VF = NOT borrow.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xy7_SUBN_VX_VY)
{
    // Test VF = 1 (no borrow) for Vy > Vx
    {
        // Arrange
        const uint8_t vxValue = 45;
        const uint8_t vyValue = 75;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8767);
        GetCPUStateRef().mRegisters[7] = vxValue;
        GetCPUStateRef().mRegisters[6] = vyValue;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t vxIndex = instruction.GetOperandX();
        const size_t vyIndex = instruction.GetOperandY();
        const uint8_t subtraction = static_cast<uint8_t>(vyValue - vxValue);

        ASSERT_EQ(7, vxIndex);
        ASSERT_EQ(6, vyIndex);
        ASSERT_EQ(subtraction, GetCPUStateRef().mRegisters[vxIndex]);
        ASSERT_EQ(1, GetCPUStateRef().mRegisters[0xF]);
    }

    // Test VF = 0 (borrow) for Vy == Vx
    {
        // Arrange
        const uint8_t vxValue = 42;
        const uint8_t vyValue = 42;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8767);
        GetCPUStateRef().mRegisters[7] = vxValue;
        GetCPUStateRef().mRegisters[6] = vyValue;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t vxIndex = instruction.GetOperandX();
        const size_t vyIndex = instruction.GetOperandY();
        const uint8_t subtraction = static_cast<uint8_t>(vyValue - vxValue);

        ASSERT_EQ(7, vxIndex);
        ASSERT_EQ(6, vyIndex);
        ASSERT_EQ(subtraction, GetCPUStateRef().mRegisters[vxIndex]);
        ASSERT_EQ(0, GetCPUStateRef().mRegisters[0xF]);
    }

    // Test VF = 0 (borrow) for Vy < Vx
    {
        // Arrange
        const uint8_t vxValue = 32;
        const uint8_t vyValue = 16;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x8767);
        GetCPUStateRef().mRegisters[7] = vxValue;
        GetCPUStateRef().mRegisters[6] = vyValue;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t vxIndex = instruction.GetOperandX();
        const size_t vyIndex = instruction.GetOperandY();
        const uint8_t subtraction = static_cast<uint8_t>(vyValue - vxValue); // Wrapping expected

        ASSERT_EQ(7, vxIndex);
        ASSERT_EQ(6, vyIndex);
        ASSERT_EQ(subtraction, GetCPUStateRef().mRegisters[vxIndex]);
        ASSERT_EQ(0, GetCPUStateRef().mRegisters[0xF]);
    }
}

// Set Vx = Vx SHL 1.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 8xyE_SHL_VX_VY)
{
    // Test VF == 1 (most-significant bit is 1)
    {
        // Arrange
        const uint8_t value = 0b10111111;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x866E);
        GetCPUStateRef().mRegisters[6] = value;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t index = instruction.GetOperandX();  // Operand Y is ignored    
    
        ASSERT_EQ(static_cast<uint8_t>(value * 2), GetCPUStateRef().mRegisters[index]);
        ASSERT_EQ(1, GetCPUStateRef().mRegisters[0xF]);
    }

    // Test VF == 0 (most-significant bit is 0)
    {
        // Arrange
        const uint8_t value = 0b00111110;

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x866E);
        GetCPUStateRef().mRegisters[6] = value;

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        const size_t index = instruction.GetOperandX();  // Operand Y is ignored    

        ASSERT_EQ(static_cast<uint8_t>(value * 2), GetCPUStateRef().mRegisters[index]);
        ASSERT_EQ(0, GetCPUStateRef().mRegisters[0xF]);
    }
}

// Skip next instruction if Vx != Vy.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 9xy0_SNE_VX_VY)
{
    // Positive case: skip next instruction
    {
        // Arrange
        mInterpreter.Reset();
        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x9560);
        GetCPUStateRef().mRegisters[5] = 0x04;
        GetCPUStateRef().mRegisters[6] = 0x05;

        // Act
        ExecuteInstruction();

        // Assert        
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }
    
    // Negative case: does not skip next instruction
    {
        // Arrange
        mInterpreter.Reset();
        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0x9560);
        GetCPUStateRef().mRegisters[5] = 0x04;
        GetCPUStateRef().mRegisters[6] = 0x04;

        // Act
        ExecuteInstruction();

        // Assert        
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }
}

// The value of register I is set to nnn.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Annn_LD_I_ADDR)
{
    // Arrange
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xA215);

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
    ASSERT_EQ(0x0215, instruction.GetOperandNNN());
    ASSERT_EQ(0x0215, GetCPUStateRef().mIndexRegister);	
}

// Jump to location nnn + V0.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Bnnn_JP_V0_ADDR)
{
    // Arrange
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xB202);
    GetCPUStateRef().mRegisters[0] = 0x02;

    // Act
    ExecuteInstruction();

    // Assert
    ASSERT_EQ(0x204, GetCPUStateRef().mProgramCounter);
}

// Set Vx = random byte AND kk.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Cxkk_RND_VX_KK)
{
    // Arrange
    const uint8_t vxIndex = 2;
    const uint8_t kk = 0x33;    
    
    EXPECT_CALL(mRandomProvider, GetRandomByte())
        .Times(1)
        .WillOnce(::testing::Return(kMockedRandomValue));
    
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xC200 | kk);

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
    ASSERT_EQ(instruction.GetOperandX(), vxIndex);
    ASSERT_EQ(instruction.GetOperandKK(), kk);
    ASSERT_EQ(GetCPUStateRef().mRegisters[vxIndex], kMockedRandomValue & kk);
}

// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Dxyn_DRW_VX_VY_N)
{
    // TODO: come back to
}

// Skip next instruction if key with the value of Vx is pressed.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Ex9E_SKP_VX)
{
    // Test skip instruction when key is pressed
    {
	    // Arrange
	    const Keypad::Key pressedKey = Keypad::Key::Key5;
	    const uint8_t value = Keypad::KeyToIndex(pressedKey);

	    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xE19E);
	    GetCPUStateRef().mRegisters[1] = value;
	    GetBusRef().mKeypad.SetKeyPressed(pressedKey, true);

	    // Act
	    const Instruction& instruction = ExecuteInstruction();

	    // Assert
	    ASSERT_EQ(1, instruction.GetOperandX());
	    ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }

    // Test do not skip instruction when key is not pressed
    {
        // Arrange
        const Keypad::Key pressedKey = Keypad::Key::Key5;
        const uint8_t value = Keypad::KeyToIndex(pressedKey);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xE19E);
        GetCPUStateRef().mRegisters[1] = value;
        GetBusRef().mKeypad.SetKeyPressed(pressedKey, false);

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        ASSERT_EQ(1, instruction.GetOperandX());
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }
}

// Skip next instruction if key with the value of Vx is not pressed.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, ExA1_SKNP_VX)
{
    // Test skip instruction when key is pressed
    {
        // Arrange
        const Keypad::Key pressedKey = Keypad::Key::Key5;
        const uint8_t value = Keypad::KeyToIndex(pressedKey);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xE1A1);
        GetCPUStateRef().mRegisters[1] = value;
        GetBusRef().mKeypad.SetKeyPressed(pressedKey, false);

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        ASSERT_EQ(1, instruction.GetOperandX());
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }

    // Test do not skip instruction when key is not pressed
    {
        // Arrange
        const Keypad::Key pressedKey = Keypad::Key::Key5;
        const uint8_t value = Keypad::KeyToIndex(pressedKey);

        WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xE1A1);
        GetCPUStateRef().mRegisters[1] = value;
        GetBusRef().mKeypad.SetKeyPressed(pressedKey, true);

        // Act
        const Instruction& instruction = ExecuteInstruction();

        // Assert
        ASSERT_EQ(1, instruction.GetOperandX());
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mProgramCounter);
    }
}

// Set Vx = delay timer value.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx07_LD_VX_DT)
{
    // Arrange
    const uint8_t value = 120;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF407);
    GetCPUStateRef().mDelayTimer = value;

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
    const size_t index = instruction.GetOperandX();

    ASSERT_EQ(4, index);
    ASSERT_EQ(value, GetCPUStateRef().mRegisters[index]);
}

// Wait for a key press, store the value of the key in Vx.
//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Fx0A_LD_VX_K)
{
    /*
        TODO: Implement test for opcode Fx0A (LD_VX_K).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

// Set delay timer = Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx15_LD_DT_VX)
{
    // Arrange
    const uint8_t value = 120;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF315);
    GetCPUStateRef().mRegisters[3] = value;

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
    ASSERT_EQ(3, instruction.GetOperandX());
    ASSERT_EQ(value, GetCPUStateRef().mDelayTimer);
}

// Set sound timer = Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx18_LD_ST_VX)
{
    // Arrange
    const uint8_t value = 120;

    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF318);
    GetCPUStateRef().mRegisters[3] = value;

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
    ASSERT_EQ(3, instruction.GetOperandX());
    ASSERT_EQ(value, GetCPUStateRef().mSoundTimer);
}

// Set I = I + Vx.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx1E_ADD_I_VX)
{
	// Arrange
	const uint16_t initialIndex = 0x100;
	const uint8_t value = 0x20; // Vx value to add

	WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF61E);
	GetCPUStateRef().mIndexRegister = initialIndex;
	GetCPUStateRef().mRegisters[6] = value;

	// Act
	const Instruction& instruction = ExecuteInstruction();

	// Assert
	ASSERT_EQ(6, instruction.GetOperandX());
	ASSERT_EQ(initialIndex + value, GetCPUStateRef().mIndexRegister);
}

// Set I = location of sprite for digit Vx.
//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Fx29_LD_F_VX)
{
    /*
        TODO: Implement test for opcode Fx29 (LD_F_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

// Store BCD representation of Vx in memory locations I, I+1, and I+2.
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Fx33_LD_B_VX)
{
	// Arrange
	const uint8_t value = 123; // BCD representation: 1, 2, 3
	const uint16_t memoryWriteStartAddress = PROGRAM_START_ADDRESS + 20;

	WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF633);
	GetCPUStateRef().mIndexRegister = memoryWriteStartAddress;
	GetCPUStateRef().mRegisters[6] = value;

	// Act
	const Instruction& instruction = ExecuteInstruction();

	// Assert
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
    
    // Arrange
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF655);
    GetCPUStateRef().mIndexRegister = memoryWriteStartAddress;
    
    for (size_t i = 0; i <= 6; i++)
    {
        GetCPUStateRef().mRegisters[i] = static_cast<uint8_t>(i * 10);
    }

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
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

    // Arrange
    WriteOpcodeAndSetPC(PROGRAM_START_ADDRESS, 0xF665);
    GetCPUStateRef().mIndexRegister = memoryReadStartAddress;

    for (uint16_t i = 0; i <= 6; i++)
    {
		WriteByteToMemory(memoryReadStartAddress + i, static_cast<uint8_t>(i * 10));        
    }

    // Act
    const Instruction& instruction = ExecuteInstruction();

    // Assert
    const size_t lastRegisterIndex = instruction.GetOperandX();
    ASSERT_EQ(6, lastRegisterIndex);

    for (uint8_t i = 0; i <= lastRegisterIndex; i++)
    {
        const uint16_t address = memoryReadStartAddress + i;
        const uint8_t expected = ReadByteFromMemory(address);
        ASSERT_EQ(expected, GetCPUStateRef().mRegisters[i]);
    }
}
