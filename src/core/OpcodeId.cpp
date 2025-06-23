#include "OpcodeId.h"

// Includes
//--------------------------------------------------------------------------------
#include <unordered_map>

//--------------------------------------------------------------------------------
std::string GetOpcodePatternString(OpcodeId id)
{
    static const std::unordered_map<OpcodeId, std::string> patternMap = {
        { OpcodeId::CLS,         "00E0" },
        { OpcodeId::RET,         "00EE" },
        { OpcodeId::JP_ADDR,     "1nnn" },
        { OpcodeId::CALL_ADDR,   "2nnn" },
        { OpcodeId::SE_VX_KK,    "3xkk" },
        { OpcodeId::SNE_VX_KK,   "4xkk" },
        { OpcodeId::SE_VX_VY,    "5xy0" },
        { OpcodeId::LD_VX_KK,    "6xkk" },
        { OpcodeId::ADD_VX_KK,   "7xkk" },
        { OpcodeId::LD_VX_VY,    "8xy0" },
        { OpcodeId::OR_VX_VY,    "8xy1" },
        { OpcodeId::AND_VX_VY,   "8xy2" },
        { OpcodeId::XOR_VX_VY,   "8xy3" },
        { OpcodeId::ADD_VX_VY,   "8xy4" },
        { OpcodeId::SUB_VX_VY,   "8xy5" },
        { OpcodeId::SHR_VX_VY,   "8xy6" },
        { OpcodeId::SUBN_VX_VY,  "8xy7" },
        { OpcodeId::SHL_VX_VY,   "8xyE" },
        { OpcodeId::SNE_VX_VY,   "9xy0" },
        { OpcodeId::LD_I_ADDR,   "Annn" },
        { OpcodeId::JP_V0_ADDR,  "Bnnn" },
        { OpcodeId::RND_VX_KK,   "Cxkk" },
        { OpcodeId::DRW_VX_VY_N, "Dxyn" },
        { OpcodeId::SKP_VX,      "Ex9E" },
        { OpcodeId::SKNP_VX,     "ExA1" },
        { OpcodeId::LD_VX_DT,    "Fx07" },
        { OpcodeId::LD_VX_K,     "Fx0A" },
        { OpcodeId::LD_DT_VX,    "Fx15" },
        { OpcodeId::LD_ST_VX,    "Fx18" },
        { OpcodeId::ADD_I_VX,    "Fx1E" },
        { OpcodeId::LD_F_VX,     "Fx29" },
        { OpcodeId::LD_B_VX,     "Fx33" },
        { OpcodeId::LD_I_VX,     "Fx55" },
        { OpcodeId::LD_VX_I,     "Fx65" }
    };

    auto it = patternMap.find(id);
    return it != patternMap.end() ? it->second : "UNKNOWN";
}
