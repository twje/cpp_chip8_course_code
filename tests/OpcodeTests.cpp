#include <gtest/gtest.h>
#define UNIT_TESTING

// Includes
//--------------------------------------------------------------------------------
// Emulator
#include "Config.h"
#include "Emulator.h"
#include "Bus.h"
#include "RAM.h"
#include "CPU.h"

//--------------------------------------------------------------------------------
class OpcodeTest : public ::testing::Test
{
protected:
    void LoadInstruction(uint16_t address, uint16_t opcode)
    {
        RAM& ram = mEmulator.mBus.mRAM;
        CPU& cpu = mEmulator.mCPU;

        // Write 2-byte opcode in big-endian format and set PC to its address
        ram.Write(address + 0, opcode >> 8);
        ram.Write(address + 1, opcode & 0xFF);
        cpu.mState.mPC = address;
    }    

    // Expose internal CPU state for test access only; this class is declared as a friend.
    CPUState& GetCPUStateRef() { return mEmulator.mCPU.mState; }

    template<typename T>
    T GetOperand(const InstructionInfo& info, size_t index)
    {
        return static_cast<T>(info.mOperands.at(index).mValue);
    }

    Emulator mEmulator;  
};

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_00E0_CLS)
{
    /*
        TODO: Implement test for opcode 00E0 (CLS).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_00EE_RET)
{
    /*
        TODO: Implement test for opcode 00EE (RET).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_1nnn_JP_ADDR)
{
    /*
        TODO: Implement test for opcode 1nnn (JP_ADDR).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

// Call subroutine at nnn
// Ref: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2nnn
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 2nnn_CALL_ADDR)
{
    // Arrange
    LoadInstruction(PROGRAM_START_ADDRESS, 0x2F23);    

    // Act
    ASSERT_EQ(ExecutionStatus::Executed, mEmulator.Step());

    // Assert
    ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mStack[0]);
    ASSERT_EQ(1, GetCPUStateRef().mSP);
    ASSERT_EQ(0x0F23, GetCPUStateRef().mPC);
}

// Skip next instruction if Vx = kk.
// Ref: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3xkk
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 3xkk_SE_VX_KK)
{
    // Positive case: V2 == 0x05, should skip next instruction
    {
        // Arrange
        LoadInstruction(PROGRAM_START_ADDRESS, 0x3205);
        GetCPUStateRef().mV[2] = 0x05;

        // Act
        ASSERT_EQ(ExecutionStatus::Executed, mEmulator.Step());
        
        // Assert        
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, GetCPUStateRef().mPC);
    }

    // Negative case: V2 != 0x05, should NOT skip
    {
        // Arrange
        mEmulator.Reset();
        LoadInstruction(PROGRAM_START_ADDRESS, 0x3205);
        GetCPUStateRef().mV[2] = 0x04;

        // Act
        ASSERT_EQ(ExecutionStatus::Executed, mEmulator.Step());
        
        // Assert        
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, GetCPUStateRef().mPC);
    }    
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_4xkk_SNE_VX_KK)
{
    /*
        TODO: Implement test for opcode 4xkk (SNE_VX_KK).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_5xy0_SE_VX_VY)
{
    /*
        TODO: Implement test for opcode 5xy0 (SE_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

// The interpreter puts the value kk into register Vx. 
// Ref: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#6xkk
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 6xkk_LD_VX_KK)
{
    // Arrange
    LoadInstruction(PROGRAM_START_ADDRESS, 0x6206);

    // Act
    InstructionInfo info = mEmulator.PreviewInstruction();
    ASSERT_EQ(ExecutionStatus::Executed, mEmulator.Step());
    
    // Assert
    const size_t vxIndex = GetOperand<size_t>(info, 0);
    const uint8_t value = GetOperand<uint8_t>(info, 1);

    ASSERT_EQ(GetCPUStateRef().mV[vxIndex], value);
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_7xkk_ADD_VX_KK)
{
    /*
        TODO: Implement test for opcode 7xkk (ADD_VX_KK).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_8xy0_LD_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy0 (LD_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_8xy1_OR_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy1 (OR_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_8xy2_AND_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy2 (AND_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_8xy3_XOR_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy3 (XOR_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_8xy4_ADD_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy4 (ADD_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_8xy5_SUB_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy5 (SUB_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_8xy6_SHR_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy6 (SHR_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_8xy7_SUBN_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy7 (SUBN_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_8xyE_SHL_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xyE (SHL_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_9xy0_SNE_VX_VY)
{
    /*
        TODO: Implement test for opcode 9xy0 (SNE_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Annn_LD_I_ADDR)
{
    /*
        TODO: Implement test for opcode Annn (LD_I_ADDR).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Bnnn_JP_V0_ADDR)
{
    /*
        TODO: Implement test for opcode Bnnn (JP_V0_ADDR).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Cxkk_RND_VX_KK)
{
    /*
        TODO: Implement test for opcode Cxkk (RND_VX_KK).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Dxyn_DRW_VX_VY_N)
{
    /*
        TODO: Implement test for opcode Dxyn (DRW_VX_VY_N).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Ex9E_SKP_VX)
{
    /*
        TODO: Implement test for opcode Ex9E (SKP_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_ExA1_SKNP_VX)
{
    /*
        TODO: Implement test for opcode ExA1 (SKNP_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Fx07_LD_VX_DT)
{
    /*
        TODO: Implement test for opcode Fx07 (LD_VX_DT).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Fx0A_LD_VX_K)
{
    /*
        TODO: Implement test for opcode Fx0A (LD_VX_K).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Fx15_LD_DT_VX)
{
    /*
        TODO: Implement test for opcode Fx15 (LD_DT_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Fx18_LD_ST_VX)
{
    /*
        TODO: Implement test for opcode Fx18 (LD_ST_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Fx1E_ADD_I_VX)
{
    /*
        TODO: Implement test for opcode Fx1E (ADD_I_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Fx29_LD_F_VX)
{
    /*
        TODO: Implement test for opcode Fx29 (LD_F_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Fx33_LD_B_VX)
{
    /*
        TODO: Implement test for opcode Fx33 (LD_B_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Fx55_LD_I_VX)
{
    /*
        TODO: Implement test for opcode Fx55 (LD_I_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Fx65_LD_VX_I)
{
    /*
        TODO: Implement test for opcode Fx65 (LD_VX_I).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}
