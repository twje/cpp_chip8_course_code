#pragma once

// Includes
//--------------------------------------------------------------------------------
// System
#include <cstdint>
#include <string>

// Reference: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#4xkk (mapping)
// This enum only covers the 3.1 base CHIP-8 instruction set.
//--------------------------------------------------------------------------------
enum class OpcodeId : uint8_t
{
    SYS_ADDR,      // 0nnn (opcode is not required for modern interpreters)
    CLS,           // 00E0
    RET,           // 00EE
    JP_ADDR,       // 1nnn
    CALL_ADDR,     // 2nnn  
    SE_VX_KK,      // 3xkk
    SNE_VX_KK,     // 4xkk
    SE_VX_VY,      // 5xy0
    LD_VX_KK,      // 6xkk
    ADD_VX_KK,     // 7xkk
    LD_VX_VY,      // 8xy0
    OR_VX_VY,      // 8xy1
    AND_VX_VY,     // 8xy2
    XOR_VX_VY,     // 8xy3
    ADD_VX_VY,     // 8xy4
    SUB_VX_VY,     // 8xy5
    SHR_VX_VY,     // 8xy6
    SUBN_VX_VY,    // 8xy7
    SHL_VX_VY,     // 8xyE
    SNE_VX_VY,     // 9xy0
    LD_I_ADDR,     // Annn
    JP_V0_ADDR,    // Bnnn
    RND_VX_KK,     // Cxkk
    DRW_VX_VY_N,   // Dxyn
    SKP_VX,        // Ex9E
    SKNP_VX,       // ExA1
    LD_VX_DT,      // Fx07
    LD_VX_K,       // Fx0A
    LD_DT_VX,      // Fx15
    LD_ST_VX,      // Fx18
    ADD_I_VX,      // Fx1E
    LD_F_VX,       // Fx29
    LD_B_VX,       // Fx33
    LD_I_VX,       // Fx55
    LD_VX_I,       // Fx65
    
    UNASSIGNED,    // Default value until pattern is decoded
};