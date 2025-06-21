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
        // Write 2-byte opcode in big-endian format and set PC to its address
        mRAM.Write(address + 0, opcode >> 8);
        mRAM.Write(address + 1, opcode & 0xFF);
        mCPU.mProgramCounter = address;
    }    

    // Expose internal CPU state for test access only; this class is declared as a friend.
    uint16_t& PCRef() { return mCPU.mProgramCounter; }
    uint16_t& IRef() { return mCPU.mIndexRegister; }
    size_t& SPRef() { return mCPU.mStackPointer; }
    uint8_t& DelayTimerRef() { return mCPU.mDelayTimer; }
    uint8_t& SoundTimerRef() { return mCPU.mSoundTimer; }

    uint8_t GetRegister(size_t index) const { return mCPU.mRegisters.at(index); }
    void SetRegister(size_t index, uint8_t value) { mCPU.mRegisters.at(index) = value; }

    uint16_t GetStackValue(size_t index) const { return mCPU.mStack.at(index); }
    void SetStackValue(size_t index, uint16_t value) { mCPU.mStack.at(index) = value; }

    Emulator mEmulator;
    RAM& mRAM = mEmulator.GetBus().mRAM;

private:
    CPU& mCPU = mEmulator.GetCPU();
};

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_00E0_CLS)
{
    /*
        TODO: Implement test for opcode 00E0 (CLS).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_00EE_RET)
{
    /*
        TODO: Implement test for opcode 00EE (RET).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_1nnn_JP_ADDR)
{
    /*
        TODO: Implement test for opcode 1nnn (JP_ADDR).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_2nnn_CALL_ADDR)
{
    /*
        TODO: Implement test for opcode 2nnn (CALL_ADDR).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, 3xkk_SE_VX_KK)
{
    // Positive case: V2 == 0x05, should skip next instruction
    {
        // Arrange
        LoadInstruction(PROGRAM_START_ADDRESS, 0x3205);
        SetRegister(2, 0x05);

        // Act
        StepResult step = mEmulator.Step();
        
        // Assert
        ASSERT_EQ(ExecutionStatus::Executed, step.mStatus);
        ASSERT_EQ(PROGRAM_START_ADDRESS + 2 * INSTRUCTION_SIZE, PCRef());
    }

    // Negative case: V2 != 0x05, should NOT skip
    {
        // Arrange
        mEmulator.Reset();
        LoadInstruction(PROGRAM_START_ADDRESS, 0x3205);
        SetRegister(2, 0x04);

        // Act
        StepResult step = mEmulator.Step();
        
        // Assert
        ASSERT_EQ(ExecutionStatus::Executed, step.mStatus);
        ASSERT_EQ(PROGRAM_START_ADDRESS + INSTRUCTION_SIZE, PCRef());
    }    
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_4xkk_SNE_VX_KK)
{
    /*
        TODO: Implement test for opcode 4xkk (SNE_VX_KK).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_5xy0_SE_VX_VY)
{
    /*
        TODO: Implement test for opcode 5xy0 (SE_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

// The interpreter puts the value kk into register Vx. 
//--------------------------------------------------------------------------------
TEST_F(OpcodeTest, Opcode_6xkk_LD_VX_KK)
{
    // Arrange
    LoadInstruction(PROGRAM_START_ADDRESS, 0x6206);    

    // Act
    const StepResult step = mEmulator.Step();
    
    // Assert
    ASSERT_EQ(ExecutionStatus::Executed, step.mStatus);

    const Instruction& instruction = step.mInstruction;
    const size_t vxIndex = instruction.GetArgument<size_t>(0);
    const uint8_t value = instruction.GetArgument<uint8_t>(1);
   
    ASSERT_EQ(GetRegister(vxIndex), value);
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_7xkk_ADD_VX_KK)
{
    /*
        TODO: Implement test for opcode 7xkk (ADD_VX_KK).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_8xy0_LD_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy0 (LD_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_8xy1_OR_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy1 (OR_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_8xy2_AND_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy2 (AND_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_8xy3_XOR_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy3 (XOR_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_8xy4_ADD_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy4 (ADD_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_8xy5_SUB_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy5 (SUB_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_8xy6_SHR_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy6 (SHR_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_8xy7_SUBN_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xy7 (SUBN_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_8xyE_SHL_VX_VY)
{
    /*
        TODO: Implement test for opcode 8xyE (SHL_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_9xy0_SNE_VX_VY)
{
    /*
        TODO: Implement test for opcode 9xy0 (SNE_VX_VY).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Annn_LD_I_ADDR)
{
    /*
        TODO: Implement test for opcode Annn (LD_I_ADDR).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Bnnn_JP_V0_ADDR)
{
    /*
        TODO: Implement test for opcode Bnnn (JP_V0_ADDR).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Cxkk_RND_VX_KK)
{
    /*
        TODO: Implement test for opcode Cxkk (RND_VX_KK).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Dxyn_DRW_VX_VY_N)
{
    /*
        TODO: Implement test for opcode Dxyn (DRW_VX_VY_N).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Ex9E_SKP_VX)
{
    /*
        TODO: Implement test for opcode Ex9E (SKP_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_ExA1_SKNP_VX)
{
    /*
        TODO: Implement test for opcode ExA1 (SKNP_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Fx07_LD_VX_DT)
{
    /*
        TODO: Implement test for opcode Fx07 (LD_VX_DT).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Fx0A_LD_VX_K)
{
    /*
        TODO: Implement test for opcode Fx0A (LD_VX_K).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Fx15_LD_DT_VX)
{
    /*
        TODO: Implement test for opcode Fx15 (LD_DT_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Fx18_LD_ST_VX)
{
    /*
        TODO: Implement test for opcode Fx18 (LD_ST_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Fx1E_ADD_I_VX)
{
    /*
        TODO: Implement test for opcode Fx1E (ADD_I_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Fx29_LD_F_VX)
{
    /*
        TODO: Implement test for opcode Fx29 (LD_F_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Fx33_LD_B_VX)
{
    /*
        TODO: Implement test for opcode Fx33 (LD_B_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Fx55_LD_I_VX)
{
    /*
        TODO: Implement test for opcode Fx55 (LD_I_VX).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}

//--------------------------------------------------------------------------------
TEST(OpcodeExecutionTests, DISABLED_Opcode_Fx65_LD_VX_I)
{
    /*
        TODO: Implement test for opcode Fx65 (LD_VX_I).
        Refer to: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    */
}
