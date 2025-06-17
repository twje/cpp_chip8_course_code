#include "InstructionSet.h"

//--------------------------------------------------------------------------------
constexpr InstructionArgDef ARG_NNN = { 0x0FFF, 0 };
constexpr InstructionArgDef ARG_KK  = { 0x00FF, 0 };
constexpr InstructionArgDef ARG_N   = { 0x000F, 0 };
constexpr InstructionArgDef ARG_X   = { 0x0F00, 8 };
constexpr InstructionArgDef ARG_Y   = { 0x00F0, 4 };

//--------------------------------------------------------------------------------
const std::vector<InstructionDef> INSTRUCTION_SET = {    
    { InstructionPatternId::CLS,         0xFFFF, 0x00E0, { } },
    { InstructionPatternId::RET,         0xFFFF, 0x00EE, { } },
    { InstructionPatternId::JP_ADDR,     0xF000, 0x1000, { ARG_NNN } },
    { InstructionPatternId::CALL_ADDR,   0xF000, 0x2000, { ARG_NNN } },
    { InstructionPatternId::SE_VX_KK,    0xF000, 0x3000, { ARG_X, ARG_KK } },
    { InstructionPatternId::SNE_VX_KK,   0xF000, 0x4000, { ARG_X, ARG_KK } },
    { InstructionPatternId::SE_VX_VY,    0xF00F, 0x5000, { ARG_X, ARG_Y } },
    { InstructionPatternId::LD_VX_KK,    0xF000, 0x6000, { ARG_X, ARG_KK } },
    { InstructionPatternId::ADD_VX_KK,   0xF000, 0x7000, { ARG_X, ARG_KK } },
    { InstructionPatternId::LD_VX_VY,    0xF00F, 0x8000, { ARG_X, ARG_Y } },
    { InstructionPatternId::OR_VX_VY,    0xF00F, 0x8001, { ARG_X, ARG_Y } },
    { InstructionPatternId::AND_VX_VY,   0xF00F, 0x8002, { ARG_X, ARG_Y } },
    { InstructionPatternId::XOR_VX_VY,   0xF00F, 0x8003, { ARG_X, ARG_Y } },
    { InstructionPatternId::ADD_VX_VY,   0xF00F, 0x8004, { ARG_X, ARG_Y } },
    { InstructionPatternId::SUB_VX_VY,   0xF00F, 0x8005, { ARG_X, ARG_Y } },
    { InstructionPatternId::SHR_VX_VY,   0xF00F, 0x8006, { ARG_X, ARG_Y } },
    { InstructionPatternId::SUBN_VX_VY,  0xF00F, 0x8007, { ARG_X, ARG_Y } },
    { InstructionPatternId::SHL_VX_VY,   0xF00F, 0x800E, { ARG_X, ARG_Y } },
    { InstructionPatternId::SNE_VX_VY,   0xF00F, 0x9000, { ARG_X, ARG_Y } },
    { InstructionPatternId::LD_I_ADDR,   0xF000, 0xA000, { ARG_NNN } },
    { InstructionPatternId::JP_V0_ADDR,  0xF000, 0xB000, { ARG_NNN } },
    { InstructionPatternId::RND_VX_KK,   0xF000, 0xC000, { ARG_X, ARG_KK } },
    { InstructionPatternId::DRW_VX_VY_N, 0xF000, 0xD000, { ARG_X, ARG_Y, ARG_N } },
    { InstructionPatternId::SKP_VX,      0xF0FF, 0xE09E, { ARG_X } },
    { InstructionPatternId::SKNP_VX,     0xF0FF, 0xE0A1, { ARG_X } },
    { InstructionPatternId::LD_VX_DT,    0xF0FF, 0xF007, { ARG_X } },
    { InstructionPatternId::LD_VX_K,     0xF0FF, 0xF00A, { ARG_X } },
    { InstructionPatternId::LD_DT_VX,    0xF0FF, 0xF015, { ARG_X } },
    { InstructionPatternId::LD_ST_VX,    0xF0FF, 0xF018, { ARG_X } },
    { InstructionPatternId::ADD_I_VX,    0xF0FF, 0xF01E, { ARG_X } },
    { InstructionPatternId::LD_F_VX,     0xF0FF, 0xF029, { ARG_X } },
    { InstructionPatternId::LD_B_VX,     0xF0FF, 0xF033, { ARG_X } },
    { InstructionPatternId::LD_I_VX,     0xF0FF, 0xF055, { ARG_X } },
    { InstructionPatternId::LD_VX_I,     0xF0FF, 0xF065, { ARG_X } },
};