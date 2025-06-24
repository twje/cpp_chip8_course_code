#include "OpcodeFormatMap.h"

//--------------------------------------------------------------------------------
constexpr OperandDef ARG_NNN = { 0x0FFF, 0, OperandLabel::NNN };
constexpr OperandDef ARG_KK = { 0x00FF, 0, OperandLabel::KK };
constexpr OperandDef ARG_N = { 0x000F, 0, OperandLabel::N };
constexpr OperandDef ARG_X = { 0x0F00, 8, OperandLabel::X };
constexpr OperandDef ARG_Y = { 0x00F0, 4, OperandLabel::Y };

//--------------------------------------------------------------------------------
const std::unordered_map<OpcodeId, OpcodeFormatDef> OPCODE_FORMAT_MAP = {
    { OpcodeId::SYS_ADDR,    { 0xF000, 0x0000, { ARG_NNN } } },
    { OpcodeId::CLS,         { 0xFFFF, 0x00E0, { } } },
    { OpcodeId::RET,         { 0xFFFF, 0x00EE, { } } },
    { OpcodeId::JP_ADDR,     { 0xF000, 0x1000, { ARG_NNN } } },
    { OpcodeId::CALL_ADDR,   { 0xF000, 0x2000, { ARG_NNN } } },
    { OpcodeId::SE_VX_KK,    { 0xF000, 0x3000, { ARG_X, ARG_KK } } },
    { OpcodeId::SNE_VX_KK,   { 0xF000, 0x4000, { ARG_X, ARG_KK } } },
    { OpcodeId::SE_VX_VY,    { 0xF00F, 0x5000, { ARG_X, ARG_Y } } },
    { OpcodeId::LD_VX_KK,    { 0xF000, 0x6000, { ARG_X, ARG_KK } } },
    { OpcodeId::ADD_VX_KK,   { 0xF000, 0x7000, { ARG_X, ARG_KK } } },
    { OpcodeId::LD_VX_VY,    { 0xF00F, 0x8000, { ARG_X, ARG_Y } } },
    { OpcodeId::OR_VX_VY,    { 0xF00F, 0x8001, { ARG_X, ARG_Y } } },
    { OpcodeId::AND_VX_VY,   { 0xF00F, 0x8002, { ARG_X, ARG_Y } } },
    { OpcodeId::XOR_VX_VY,   { 0xF00F, 0x8003, { ARG_X, ARG_Y } } },
    { OpcodeId::ADD_VX_VY,   { 0xF00F, 0x8004, { ARG_X, ARG_Y } } },
    { OpcodeId::SUB_VX_VY,   { 0xF00F, 0x8005, { ARG_X, ARG_Y } } },
    { OpcodeId::SHR_VX_VY,   { 0xF00F, 0x8006, { ARG_X, ARG_Y } } },
    { OpcodeId::SUBN_VX_VY,  { 0xF00F, 0x8007, { ARG_X, ARG_Y } } },
    { OpcodeId::SHL_VX_VY,   { 0xF00F, 0x800E, { ARG_X, ARG_Y } } },
    { OpcodeId::SNE_VX_VY,   { 0xF00F, 0x9000, { ARG_X, ARG_Y } } },
    { OpcodeId::LD_I_ADDR,   { 0xF000, 0xA000, { ARG_NNN } } },
    { OpcodeId::JP_V0_ADDR,  { 0xF000, 0xB000, { ARG_NNN } } },
    { OpcodeId::RND_VX_KK,   { 0xF000, 0xC000, { ARG_X, ARG_KK } } },
    { OpcodeId::DRW_VX_VY_N, { 0xF000, 0xD000, { ARG_X, ARG_Y, ARG_N } } },
    { OpcodeId::SKP_VX,      { 0xF0FF, 0xE09E, { ARG_X } } },
    { OpcodeId::SKNP_VX,     { 0xF0FF, 0xE0A1, { ARG_X } } },
    { OpcodeId::LD_VX_DT,    { 0xF0FF, 0xF007, { ARG_X } } },
    { OpcodeId::LD_VX_K,     { 0xF0FF, 0xF00A, { ARG_X } } },
    { OpcodeId::LD_DT_VX,    { 0xF0FF, 0xF015, { ARG_X } } },
    { OpcodeId::LD_ST_VX,    { 0xF0FF, 0xF018, { ARG_X } } },
    { OpcodeId::ADD_I_VX,    { 0xF0FF, 0xF01E, { ARG_X } } },
    { OpcodeId::LD_F_VX,     { 0xF0FF, 0xF029, { ARG_X } } },
    { OpcodeId::LD_B_VX,     { 0xF0FF, 0xF033, { ARG_X } } },
    { OpcodeId::LD_I_VX,     { 0xF0FF, 0xF055, { ARG_X } } },
    { OpcodeId::LD_VX_I,     { 0xF0FF, 0xF065, { ARG_X } } },
};

// Operand labels use lowercase (e.g., "nnn") to denote placeholders and avoid confusion 
// with uppercase hex digits.
//--------------------------------------------------------------------------------
std::string OperandLabelToString(OperandLabel label)
{
    switch (label)
    {
        case OperandLabel::NNN: return "nnn";
        case OperandLabel::KK:  return "kk";
        case OperandLabel::N:   return "n";
        case OperandLabel::X:   return "x";
        case OperandLabel::Y:   return "y";
        default: return "none";
    }
}