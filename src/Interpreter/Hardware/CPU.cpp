#include "Interpreter/Hardware/CPU.h"

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Constants.h"
#include "Interpreter/Hardware/DelayTimer.h"
#include "Interpreter/Hardware/Display.h"
#include "Interpreter/Instruction/Instruction.h"
#include "Interpreter/Instruction/OpcodeTable.h"
#include "Interpreter/Hardware/Keypad.h"
#include "Interpreter/Hardware/RAM.h"
#include "Interpreter/Hardware/SoundTimer.h"

// System
#include <cassert>

//--------------------------------------------------------------------------------
CPU::CPU(Bus& bus, IRandomProvider& randomProvider)
    : mBus{ bus }
    , mRandomProvider(randomProvider)
{ 
    mState.mProgramCounter = PROGRAM_START_ADDRESS;
}

//--------------------------------------------------------------------------------
void CPU::Reset()
{
    mState.mProgramCounter = PROGRAM_START_ADDRESS;
    mState.mIndexRegister = 0;
    mState.mRegisters.fill(0);
    mState.mStackPointer = 0;
    mState.mStack.fill(0);
    mState.mDelayTimer = 0;
    mState.mSoundTimer = 0;
}

//--------------------------------------------------------------------------------
void CPU::DecrementTimers()
{
    if (mState.mDelayTimer > 0)
    {
        --mState.mDelayTimer;
	}

    if (mState.mSoundTimer > 0)
    {
        --mState.mSoundTimer;
    }
}

//--------------------------------------------------------------------------------
FetchResult CPU::Peek() const
{
    FetchResult result;

    const uint16_t address = mState.mProgramCounter;    

    if (address % 2 != 0)
    {
        result.mStatus = ExecutionStatus::InvalidAddressUnaligned;
        result.mIsValidAddress = false;
        return result;
    }

    if (address < PROGRAM_START_ADDRESS || (address + 1) >= RAM_SIZE)
    {
        result.mStatus = ExecutionStatus::InvalidAddressOutOfBounds;
        result.mIsValidAddress = false;
        return result;
    }

    const uint8_t highByte = mBus.mRAM.Read(address);
    const uint8_t lowByte = mBus.mRAM.Read(address + 1);
    result.mOpcode = (static_cast<uint16_t>(highByte) << 8) | lowByte;

    return result;
}

//--------------------------------------------------------------------------------
FetchResult CPU::Fetch()
{
	FetchResult result = Peek();
	mState.mProgramCounter += INSTRUCTION_SIZE;

    return result;
}

//--------------------------------------------------------------------------------
Instruction CPU::Decode(uint16_t opcode) const
{
    for (const OpcodeSpec& opcodeSpec : OpcodeTable::All())
    {
        // Apply the mask to isolate pattern-relevant bits, then compare to expected pattern
        // e.g. (0x8123 & 0xF00F) == 0x8003 for XOR_VX_VY
        if ((opcode & opcodeSpec.mMask) == opcodeSpec.mPattern)
        {
            // Parse opcode operands
            std::vector<uint16_t> operands;            
            for (const auto& operandSpec : opcodeSpec.mOperands)
            {
                uint16_t value = (opcode & operandSpec.mMask) >> operandSpec.mShift;
                operands.push_back(value);
            }
        
            return { opcodeSpec.mOpcodeId, operands };            
        }
    }

	return { }; // Decode failed, return an empty instruction
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute(const Instruction& instruction)
{
    assert(instruction.IsValid());

    ExecutionStatus status = ExecutionStatus::MissingHandler;

    switch (instruction.GetOpcodeId())
    {
        case OpcodeId::SYS_ADDR:    status = Execute_0nnn_SYS_ADDR(instruction); break;
        case OpcodeId::CLS:         status = Execute_00E0_CLS(instruction); break;
        case OpcodeId::RET:         status = Execute_00EE_RET(instruction); break;
        case OpcodeId::JP_ADDR:     status = Execute_1nnn_JP_ADDR(instruction); break;
        case OpcodeId::CALL_ADDR:   status = Execute_2nnn_CALL_ADDR(instruction); break;
        case OpcodeId::SE_VX_KK:    status = Execute_3xkk_SE_VX_KK(instruction); break;
        case OpcodeId::SNE_VX_KK:   status = Execute_4xkk_SNE_VX_KK(instruction); break;
        case OpcodeId::SE_VX_VY:    status = Execute_5xy0_SE_VX_VY(instruction); break;
        case OpcodeId::LD_VX_KK:    status = Execute_6xkk_LD_VX_KK(instruction); break;
        case OpcodeId::ADD_VX_KK:   status = Execute_7xkk_ADD_VX_KK(instruction); break;
        case OpcodeId::LD_VX_VY:    status = Execute_8xy0_LD_VX_VY(instruction); break;
        case OpcodeId::OR_VX_VY:    status = Execute_8xy1_OR_VX_VY(instruction); break;
        case OpcodeId::AND_VX_VY:   status = Execute_8xy2_AND_VX_VY(instruction); break;
        case OpcodeId::XOR_VX_VY:   status = Execute_8xy3_XOR_VX_VY(instruction); break;
        case OpcodeId::ADD_VX_VY:   status = Execute_8xy4_ADD_VX_VY(instruction); break;
        case OpcodeId::SUB_VX_VY:   status = Execute_8xy5_SUB_VX_VY(instruction); break;
        case OpcodeId::SHR_VX_VY:   status = Execute_8xy6_SHR_VX_VY(instruction); break;
        case OpcodeId::SUBN_VX_VY:  status = Execute_8xy7_SUBN_VX_VY(instruction); break;
        case OpcodeId::SHL_VX_VY:   status = Execute_8xyE_SHL_VX_VY(instruction); break;
        case OpcodeId::SNE_VX_VY:   status = Execute_9xy0_SNE_VX_VY(instruction); break;
        case OpcodeId::LD_I_ADDR:   status = Execute_Annn_LD_I_ADDR(instruction); break;
        case OpcodeId::JP_V0_ADDR:  status = Execute_Bnnn_JP_V0_ADDR(instruction); break;
        case OpcodeId::RND_VX_KK:   status = Execute_Cxkk_RND_VX_KK(instruction); break;
        case OpcodeId::DRW_VX_VY_N: status = Execute_Dxyn_DRW_VX_VY_N(instruction); break;
        case OpcodeId::SKP_VX:      status = Execute_Ex9E_SKP_VX(instruction); break;
        case OpcodeId::SKNP_VX:     status = Execute_ExA1_SKNP_VX(instruction); break;
        case OpcodeId::LD_VX_DT:    status = Execute_Fx07_LD_VX_DT(instruction); break;
        case OpcodeId::LD_VX_K:     status = Execute_Fx0A_LD_VX_K(instruction); break;
        case OpcodeId::LD_DT_VX:    status = Execute_Fx15_LD_DT_VX(instruction); break;
        case OpcodeId::LD_ST_VX:    status = Execute_Fx18_LD_ST_VX(instruction); break;
        case OpcodeId::ADD_I_VX:    status = Execute_Fx1E_ADD_I_VX(instruction); break;
        case OpcodeId::LD_F_VX:     status = Execute_Fx29_LD_F_VX(instruction); break;
        case OpcodeId::LD_B_VX:     status = Execute_Fx33_LD_B_VX(instruction); break;
        case OpcodeId::LD_I_VX:     status = Execute_Fx55_LD_I_VX(instruction); break;
        case OpcodeId::LD_VX_I:     status = Execute_Fx65_LD_VX_I(instruction); break;
    } 

    return status;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_0nnn_SYS_ADDR(const Instruction&)
{
    /*
        NOTE: Legacy SYS instruction (0nnn); ignored in modern interpreters.        
    */

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_00E0_CLS(const Instruction& instruction)
{
	(void)instruction; // Unused parameter
	mBus.mDisplay.Clear();

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_00EE_RET(const Instruction&)
{   
    mState.mStackPointer--;
    mState.mProgramCounter = mState.mStack[mState.mStackPointer];    

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_1nnn_JP_ADDR(const Instruction& instruction)
{
    const uint16_t address = instruction.GetOperandNNN();
    mState.mProgramCounter = address;

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_2nnn_CALL_ADDR(const Instruction& instruction)
{
    const uint16_t address = instruction.GetOperandNNN();

    mState.mStack[mState.mStackPointer] = mState.mProgramCounter;
    mState.mStackPointer++;
    mState.mProgramCounter = address;
 
    return ExecutionStatus::Executed;
}

// Skip next instruction if Vx = kk.
//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_3xkk_SE_VX_KK(const Instruction& instruction)
{
    const size_t reg = instruction.GetOperandX();
    const uint8_t value = instruction.GetOperandKK();
        
    if (mState.mRegisters.at(reg) == value)
    {
        mState.mProgramCounter += INSTRUCTION_SIZE;
    }

    return ExecutionStatus::Executed;
}

// Skip next instruction if Vx != kk.
//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_4xkk_SNE_VX_KK(const Instruction& instruction)
{
    const size_t reg = instruction.GetOperandX();
    const uint8_t value = instruction.GetOperandKK();    

    if (mState.mRegisters.at(reg) != value)
    {
        mState.mProgramCounter += INSTRUCTION_SIZE;
    }

    return ExecutionStatus::Executed;
}

// Skip next instruction if Vx = Vy.
//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_5xy0_SE_VX_VY(const Instruction& instruction)
{
    const size_t vxReg = instruction.GetOperandX();
    const size_t vyReg = instruction.GetOperandY();
    
    if (mState.mRegisters[vxReg] == mState.mRegisters[vyReg])
    {
        mState.mProgramCounter += INSTRUCTION_SIZE;
    }

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_6xkk_LD_VX_KK(const Instruction& instruction)
{
    const size_t reg = instruction.GetOperandX();
    const uint8_t value = instruction.GetOperandKK();
    
    mState.mRegisters[reg] = value;

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_7xkk_ADD_VX_KK(const Instruction& instruction)
{
    const size_t reg = instruction.GetOperandX();
    const uint8_t value = instruction.GetOperandKK();
    
    mState.mRegisters[reg] += value;

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy0_LD_VX_VY(const Instruction& instruction)
{
    const size_t vxReg = instruction.GetOperandX();
    const size_t vyReg = instruction.GetOperandY();
        
    mState.mRegisters[vxReg] = mState.mRegisters[vyReg];

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy1_OR_VX_VY(const Instruction& instruction)
{
    const size_t vxReg = instruction.GetOperandX();
    const size_t vyReg = instruction.GetOperandY();    

    mState.mRegisters[vxReg] = mState.mRegisters[vxReg] | mState.mRegisters[vyReg];

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy2_AND_VX_VY(const Instruction& instruction)
{
    const size_t vxReg = instruction.GetOperandX();
    const size_t vyReg = instruction.GetOperandY();

    mState.mRegisters[vxReg] = mState.mRegisters[vxReg] & mState.mRegisters[vyReg];

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy3_XOR_VX_VY(const Instruction& instruction)
{
    const size_t vxReg = instruction.GetOperandX();
    const size_t vyReg = instruction.GetOperandY();

    mState.mRegisters[vxReg] = mState.mRegisters[vxReg] ^ mState.mRegisters[vyReg];

    return ExecutionStatus::Executed;
}

// Set Vx = Vx + Vy, set VF = carry.
//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy4_ADD_VX_VY(const Instruction& instruction)
{
    const size_t vxReg = instruction.GetOperandX();
    const size_t vyReg = instruction.GetOperandY();

    const uint16_t sum = mState.mRegisters[vxReg] + mState.mRegisters[vyReg];
    const uint8_t carry = sum > 255 ? 1 : 0;

    mState.mRegisters[vxReg] = static_cast<uint8_t>(sum & 0xFF);
    mState.mRegisters[0xF] = carry;

    return ExecutionStatus::Executed;
}

// Set Vx = Vx - Vy, set VF = NOT borrow.
//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy5_SUB_VX_VY(const Instruction& instruction)
{
    const size_t vxReg = instruction.GetOperandX();
    const size_t vyReg = instruction.GetOperandY();

	const uint16_t vxValue = mState.mRegisters[vxReg];
	const uint16_t vyValue = mState.mRegisters[vyReg];

    const uint8_t noBorrow = (vxValue >= vyValue) ? 1 : 0;
    
    mState.mRegisters[vxReg] = static_cast<uint8_t>(vxValue - vyValue);
    mState.mRegisters[0xF] = noBorrow;

    return ExecutionStatus::Executed;
}

// Set Vx = Vx SHR 1.
//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy6_SHR_VX_VY(const Instruction& instruction)
{
    /*
        NOTE: In original CHIP-8, Vy is ignored for 8xy6.
        Vx is both the input and output register.
    */

    const size_t reg = instruction.GetOperandX();
    const uint8_t value = mState.mRegisters[reg];
	const uint8_t lsb = value & 0x01;

    mState.mRegisters[reg] = value >> 1;
    mState.mRegisters[0xF] = lsb;

    return ExecutionStatus::Executed;
}

// Set Vx = Vy - Vx, set VF = NOT borrow.
//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xy7_SUBN_VX_VY(const Instruction& instruction)
{
    const size_t vxReg = instruction.GetOperandX();
    const size_t vyReg = instruction.GetOperandY();

    const uint16_t vxValue = mState.mRegisters[vxReg];
    const uint16_t vyValue = mState.mRegisters[vyReg];
    
    const uint8_t noBorrow = (vyValue >= vxValue) ? 1 : 0;
    
    mState.mRegisters[vxReg] = static_cast<uint8_t>(vyValue - vxValue);
    mState.mRegisters[0xF] = noBorrow;

    return ExecutionStatus::Executed;
}

// Set Vx = Vx SHL 1.
//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_8xyE_SHL_VX_VY(const Instruction& instruction)
{
    /*
        NOTE: In original CHIP-8, Vy is ignored for 8xyE.
        Vx is both the input and output register.
    */

    const size_t reg = instruction.GetOperandX();    
    const uint8_t value = mState.mRegisters[reg];	
    const uint8_t msb = (value & 0x80) >> 7;

	mState.mRegisters[reg] = value << 1;
    mState.mRegisters[0xF] = msb;

    return ExecutionStatus::Executed;
}

// Skip next instruction if Vx != Vy.
//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_9xy0_SNE_VX_VY(const Instruction& instruction)
{
    const size_t vxReg = instruction.GetOperandX();
    const size_t vyReg = instruction.GetOperandY();

    if (mState.mRegisters[vxReg] != mState.mRegisters[vyReg])
    {
        mState.mProgramCounter += INSTRUCTION_SIZE;
    }

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Annn_LD_I_ADDR(const Instruction& instruction)
{
    const uint16_t value = instruction.GetOperandNNN();
    mState.mIndexRegister = value;

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Bnnn_JP_V0_ADDR(const Instruction& instruction)
{
    const uint16_t address = instruction.GetOperandNNN();
	const uint8_t offset = mState.mRegisters[0];

    mState.mProgramCounter = address + offset;

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Cxkk_RND_VX_KK(const Instruction& instruction)
{
    const size_t reg = instruction.GetOperandX();
	const uint8_t value = instruction.GetOperandKK();

	mState.mRegisters[reg] = mRandomProvider.GetRandomByte() & value;

    return ExecutionStatus::Executed;
}

// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Dxyn_DRW_VX_VY_N(const Instruction& instruction)
{
    const size_t vxReg = instruction.GetOperandX();
    const size_t vyReg = instruction.GetOperandY();
    const uint8_t height = instruction.GetOperandN();
       
    mState.mRegisters[0xF] = mBus.mDisplay.DrawSprite(
        mState.mRegisters[vxReg],
        mState.mRegisters[vyReg],
        mState.mIndexRegister,
        height
    );    

    return ExecutionStatus::Executed;
}

// Skip next instruction if key with the value of Vx is pressed.
//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Ex9E_SKP_VX(const Instruction& instruction)
{
    const size_t reg = instruction.GetOperandX();
	uint8_t key = mState.mRegisters[reg];
	
	if (mBus.mKeypad.IsKeyPressed(Keypad::IndexToKey(key)))
	{
		mState.mProgramCounter += INSTRUCTION_SIZE; // Skip next instruction
	}

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_ExA1_SKNP_VX(const Instruction& instruction)
{
    const size_t reg = instruction.GetOperandX();
    uint8_t key = mState.mRegisters[reg];

    if (!mBus.mKeypad.IsKeyPressed(Keypad::IndexToKey(key)))
    {
        mState.mProgramCounter += INSTRUCTION_SIZE; // Skip next instruction
    }

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx07_LD_VX_DT(const Instruction& instruction)
{
    const size_t reg = instruction.GetOperandX();

    mState.mRegisters[reg] = mState.mDelayTimer;

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx0A_LD_VX_K(const Instruction& instruction)
{
    /*
        Hint: Fx0A is the only opcode that waits for input.
        Return WaitingOnKeyPress to pause execution and retry this instruction next cycle.
		See the unit test for example behavior.

        Note: Fx0A waits for a key release, not a key press.
        (See errata: https://github.com/gulrak/cadmium/wiki/CTR-Errata#fx0a)
    */

    const size_t reg = instruction.GetOperandX();    
    auto releasedKey = mBus.mKeypad.GetFirstKeyReleased();
        
    if (!releasedKey.has_value())
    {
		// No key pressed; roll back PC to retry this instruction        
        return ExecutionStatus::WaitingOnKeyPress;
    }
    
	mState.mRegisters[reg] = Keypad::KeyToIndex(releasedKey.value());
    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx15_LD_DT_VX(const Instruction& instruction)
{
    const size_t reg = instruction.GetOperandX();

    mState.mDelayTimer = mState.mRegisters[reg];

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx18_LD_ST_VX(const Instruction& instruction)
{
    const size_t reg = instruction.GetOperandX();

    mState.mSoundTimer = mState.mRegisters[reg];

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx1E_ADD_I_VX(const Instruction& instruction)
{
    const size_t reg = instruction.GetOperandX();    
	
    mState.mIndexRegister += mState.mRegisters[reg];

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx29_LD_F_VX(const Instruction& instruction)
{
    /*
        Hint: Fx29 sets I to the address of the font sprite for digit in Vx (0x0–0xF).
        Each digit's sprite is 5 bytes, starting at address 0x000.
        See the unit test for example behavior.
    */
    
    const size_t reg = instruction.GetOperandX();
    const uint8_t value = mState.mRegisters[reg];

    // validate value is within valid digit range (0–F)
    assert(value <= 0x0F && "Fx29 expects Vx to contain a hexadecimal digit (0x0–0xF)");

	mState.mIndexRegister = value * kFontSpriteSize;

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx33_LD_B_VX(const Instruction& instruction)
{    
    const size_t reg = instruction.GetOperandX();
	const uint8_t value = mState.mRegisters[reg];
	const uint16_t address = mState.mIndexRegister;

    const uint8_t hundreds = value / 100;
    const uint8_t tens = (value / 10) % 10;
    const uint8_t units = value % 10;

    mBus.mRAM.Write(address + 0, hundreds);
    mBus.mRAM.Write(address + 1, tens);
    mBus.mRAM.Write(address + 2, units);

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx55_LD_I_VX(const Instruction& instruction)
{
    const size_t lastRegisterIndex = instruction.GetOperandX();

    for (size_t reg = 0; reg <= lastRegisterIndex; ++reg)
    {
        const uint16_t address = mState.mIndexRegister + static_cast<uint16_t>(reg);
        mBus.mRAM.Write(address, mState.mRegisters[reg]);
    }

    return ExecutionStatus::Executed;
}

//--------------------------------------------------------------------------------
ExecutionStatus CPU::Execute_Fx65_LD_VX_I(const Instruction& instruction)
{
    const size_t lastRegisterIndex = instruction.GetOperandX();

    for (size_t reg = 0; reg <= lastRegisterIndex; ++reg)
    {
        const uint16_t address = mState.mIndexRegister + static_cast<uint16_t>(reg);
        mState.mRegisters[reg] = mBus.mRAM.Read(address);
    }

    return ExecutionStatus::Executed;
}