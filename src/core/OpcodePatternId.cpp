#include "OpcodePatternId.h"

// Includes
//--------------------------------------------------------------------------------
#include <unordered_map>

//--------------------------------------------------------------------------------
std::string GetOpcodePatternString(OpcodePatternId id)
{
    static const std::unordered_map<OpcodePatternId, std::string> patternMap = {
        { OpcodePatternId::CLS,         "00E0" },
        { OpcodePatternId::RET,         "00EE" },
        { OpcodePatternId::JP_ADDR,     "1nnn" },
        { OpcodePatternId::CALL_ADDR,   "2nnn" },
        { OpcodePatternId::SE_VX_KK,    "3xkk" },
        { OpcodePatternId::SNE_VX_KK,   "4xkk" },
        { OpcodePatternId::SE_VX_VY,    "5xy0" },
        { OpcodePatternId::LD_VX_KK,    "6xkk" },
        { OpcodePatternId::ADD_VX_KK,   "7xkk" },
        { OpcodePatternId::LD_VX_VY,    "8xy0" },
        { OpcodePatternId::OR_VX_VY,    "8xy1" },
        { OpcodePatternId::AND_VX_VY,   "8xy2" },
        { OpcodePatternId::XOR_VX_VY,   "8xy3" },
        { OpcodePatternId::ADD_VX_VY,   "8xy4" },
        { OpcodePatternId::SUB_VX_VY,   "8xy5" },
        { OpcodePatternId::SHR_VX_VY,   "8xy6" },
        { OpcodePatternId::SUBN_VX_VY,  "8xy7" },
        { OpcodePatternId::SHL_VX_VY,   "8xyE" },
        { OpcodePatternId::SNE_VX_VY,   "9xy0" },
        { OpcodePatternId::LD_I_ADDR,   "Annn" },
        { OpcodePatternId::JP_V0_ADDR,  "Bnnn" },
        { OpcodePatternId::RND_VX_KK,   "Cxkk" },
        { OpcodePatternId::DRW_VX_VY_N, "Dxyn" },
        { OpcodePatternId::SKP_VX,      "Ex9E" },
        { OpcodePatternId::SKNP_VX,     "ExA1" },
        { OpcodePatternId::LD_VX_DT,    "Fx07" },
        { OpcodePatternId::LD_VX_K,     "Fx0A" },
        { OpcodePatternId::LD_DT_VX,    "Fx15" },
        { OpcodePatternId::LD_ST_VX,    "Fx18" },
        { OpcodePatternId::ADD_I_VX,    "Fx1E" },
        { OpcodePatternId::LD_F_VX,     "Fx29" },
        { OpcodePatternId::LD_B_VX,     "Fx33" },
        { OpcodePatternId::LD_I_VX,     "Fx55" },
        { OpcodePatternId::LD_VX_I,     "Fx65" }
    };

    auto it = patternMap.find(id);
    return it != patternMap.end() ? it->second : "UNKNOWN";
}
