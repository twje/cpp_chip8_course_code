#include "OpcodeTable.h"

//------------------------------------------------------------------------------
constexpr OperandSpec ARG_NNN{ 0x0FFF, 0, "nnn" };
constexpr OperandSpec ARG_KK{ 0x00FF, 0, "kk" };
constexpr OperandSpec ARG_N{ 0x000F, 0, "n" };
constexpr OperandSpec ARG_X{ 0x0F00, 8, "x" };
constexpr OperandSpec ARG_Y{ 0x00F0, 4, "y" };

//------------------------------------------------------------------------------
const std::unordered_map<OpcodeId, OpcodeSpec> OPCODE_TABLE = {
    { OpcodeId::SYS_ADDR,    { 0xF000, 0x0000, "0nnn", "SYS",  { ARG_NNN } } },
    { OpcodeId::CLS,         { 0xFFFF, 0x00E0, "00E0", "CLS",  { } } },
    { OpcodeId::RET,         { 0xFFFF, 0x00EE, "00EE", "RET",  { } } },
    { OpcodeId::JP_ADDR,     { 0xF000, 0x1000, "1nnn", "JP",   { ARG_NNN } } },
    { OpcodeId::CALL_ADDR,   { 0xF000, 0x2000, "2nnn", "CALL", { ARG_NNN } } },
    { OpcodeId::SE_VX_KK,    { 0xF000, 0x3000, "3xkk", "SE",   { ARG_X, ARG_KK } } },
    { OpcodeId::SNE_VX_KK,   { 0xF000, 0x4000, "4xkk", "SNE",  { ARG_X, ARG_KK } } },
    { OpcodeId::SE_VX_VY,    { 0xF00F, 0x5000, "5xy0", "SE",   { ARG_X, ARG_Y } } },
    { OpcodeId::LD_VX_KK,    { 0xF000, 0x6000, "6xkk", "LD",   { ARG_X, ARG_KK } } },
    { OpcodeId::ADD_VX_KK,   { 0xF000, 0x7000, "7xkk", "ADD",  { ARG_X, ARG_KK } } },
    { OpcodeId::LD_VX_VY,    { 0xF00F, 0x8000, "8xy0", "LD",   { ARG_X, ARG_Y } } },
    { OpcodeId::OR_VX_VY,    { 0xF00F, 0x8001, "8xy1", "OR",   { ARG_X, ARG_Y } } },
    { OpcodeId::AND_VX_VY,   { 0xF00F, 0x8002, "8xy2", "AND",  { ARG_X, ARG_Y } } },
    { OpcodeId::XOR_VX_VY,   { 0xF00F, 0x8003, "8xy3", "XOR",  { ARG_X, ARG_Y } } },
    { OpcodeId::ADD_VX_VY,   { 0xF00F, 0x8004, "8xy4", "ADD",  { ARG_X, ARG_Y } } },
    { OpcodeId::SUB_VX_VY,   { 0xF00F, 0x8005, "8xy5", "SUB",  { ARG_X, ARG_Y } } },
    { OpcodeId::SHR_VX_VY,   { 0xF00F, 0x8006, "8xy6", "SHR",  { ARG_X, ARG_Y } } },
    { OpcodeId::SUBN_VX_VY,  { 0xF00F, 0x8007, "8xy7", "SUBN", { ARG_X, ARG_Y } } },
    { OpcodeId::SHL_VX_VY,   { 0xF00F, 0x800E, "8xyE", "SHL",  { ARG_X, ARG_Y } } },
    { OpcodeId::SNE_VX_VY,   { 0xF00F, 0x9000, "9xy0", "SNE",  { ARG_X, ARG_Y } } },
    { OpcodeId::LD_I_ADDR,   { 0xF000, 0xA000, "Annn", "LD",   { ARG_NNN } } },
    { OpcodeId::JP_V0_ADDR,  { 0xF000, 0xB000, "Bnnn", "JP",   { ARG_NNN } } },
    { OpcodeId::RND_VX_KK,   { 0xF000, 0xC000, "Cxkk", "RND",  { ARG_X, ARG_KK } } },
    { OpcodeId::DRW_VX_VY_N, { 0xF000, 0xD000, "Dxyn", "DRW",  { ARG_X, ARG_Y, ARG_N } } },
    { OpcodeId::SKP_VX,      { 0xF0FF, 0xE09E, "Ex9E", "SKP",  { ARG_X } } },
    { OpcodeId::SKNP_VX,     { 0xF0FF, 0xE0A1, "ExA1", "SKNP", { ARG_X } } },
    { OpcodeId::LD_VX_DT,    { 0xF0FF, 0xF007, "Fx07", "LD",   { ARG_X } } },
    { OpcodeId::LD_VX_K,     { 0xF0FF, 0xF00A, "Fx0A", "LD",   { ARG_X } } },
    { OpcodeId::LD_DT_VX,    { 0xF0FF, 0xF015, "Fx15", "LD",   { ARG_X } } },
    { OpcodeId::LD_ST_VX,    { 0xF0FF, 0xF018, "Fx18", "LD",   { ARG_X } } },
    { OpcodeId::ADD_I_VX,    { 0xF0FF, 0xF01E, "Fx1E", "ADD",  { ARG_X } } },
    { OpcodeId::LD_F_VX,     { 0xF0FF, 0xF029, "Fx29", "LD",   { ARG_X } } },
    { OpcodeId::LD_B_VX,     { 0xF0FF, 0xF033, "Fx33", "LD",   { ARG_X } } },
    { OpcodeId::LD_I_VX,     { 0xF0FF, 0xF055, "Fx55", "LD",   { ARG_X } } },
    { OpcodeId::LD_VX_I,     { 0xF0FF, 0xF065, "Fx65", "LD",   { ARG_X } } },
};