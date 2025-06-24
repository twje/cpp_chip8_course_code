#include "OpcodeId.h"

// Includes
//--------------------------------------------------------------------------------
#include <unordered_map>

OpcodeDisplayInfo GetOpcodeDisplayInfo(OpcodeId id)
{
    static const std::unordered_map<OpcodeId, OpcodeDisplayInfo> displayMap = {
        { OpcodeId::CLS,         { "00E0", "CLS" } },
        { OpcodeId::RET,         { "00EE", "RET" } },
        { OpcodeId::JP_ADDR,     { "1nnn", "JP"  } },
        { OpcodeId::CALL_ADDR,   { "2nnn", "CALL"} },
        { OpcodeId::SE_VX_KK,    { "3xkk", "SE"  } },
        { OpcodeId::SNE_VX_KK,   { "4xkk", "SNE" } },
        { OpcodeId::SE_VX_VY,    { "5xy0", "SE"  } },
        { OpcodeId::LD_VX_KK,    { "6xkk", "LD"  } },
        { OpcodeId::ADD_VX_KK,   { "7xkk", "ADD" } },
        { OpcodeId::LD_VX_VY,    { "8xy0", "LD"  } },
        { OpcodeId::OR_VX_VY,    { "8xy1", "OR"  } },
        { OpcodeId::AND_VX_VY,   { "8xy2", "AND" } },
        { OpcodeId::XOR_VX_VY,   { "8xy3", "XOR" } },
        { OpcodeId::ADD_VX_VY,   { "8xy4", "ADD" } },
        { OpcodeId::SUB_VX_VY,   { "8xy5", "SUB" } },
        { OpcodeId::SHR_VX_VY,   { "8xy6", "SHR" } },
        { OpcodeId::SUBN_VX_VY,  { "8xy7", "SUBN"} },
        { OpcodeId::SHL_VX_VY,   { "8xyE", "SHL" } },
        { OpcodeId::SNE_VX_VY,   { "9xy0", "SNE" } },
        { OpcodeId::LD_I_ADDR,   { "Annn", "LD"  } },
        { OpcodeId::JP_V0_ADDR,  { "Bnnn", "JP"  } },
        { OpcodeId::RND_VX_KK,   { "Cxkk", "RND" } },
        { OpcodeId::DRW_VX_VY_N, { "Dxyn", "DRW" } },
        { OpcodeId::SKP_VX,      { "Ex9E", "SKP" } },
        { OpcodeId::SKNP_VX,     { "ExA1", "SKNP"} },
        { OpcodeId::LD_VX_DT,    { "Fx07", "LD"  } },
        { OpcodeId::LD_VX_K,     { "Fx0A", "LD"  } },
        { OpcodeId::LD_DT_VX,    { "Fx15", "LD"  } },
        { OpcodeId::LD_ST_VX,    { "Fx18", "LD"  } },
        { OpcodeId::ADD_I_VX,    { "Fx1E", "ADD" } },
        { OpcodeId::LD_F_VX,     { "Fx29", "LD"  } },
        { OpcodeId::LD_B_VX,     { "Fx33", "LD"  } },
        { OpcodeId::LD_I_VX,     { "Fx55", "LD"  } },
        { OpcodeId::LD_VX_I,     { "Fx65", "LD"  } }
    };

    auto it = displayMap.find(id);
    if (it != displayMap.end())
    {
        return it->second;
    }

    return { "????", "UNKNOWN" };
}

