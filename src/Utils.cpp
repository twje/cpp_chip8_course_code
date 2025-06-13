#include "Utils.h"

// Includes
//--------------------------------------------------------------------------------
#include <unordered_map>

// Macros
//--------------------------------------------------------------------------------
#define OPCODE_ID_STRING_ENTRY(name) { OpCodeId::name, #name }

//--------------------------------------------------------------------------------
std::string OpCodeIdToString(OpCodeId id)
{   
    static const std::unordered_map<OpCodeId, std::string> map = {
        OPCODE_ID_STRING_ENTRY(CLS),
        OPCODE_ID_STRING_ENTRY(RET),
        OPCODE_ID_STRING_ENTRY(JP_ADDR),
        OPCODE_ID_STRING_ENTRY(CALL_ADDR),
        OPCODE_ID_STRING_ENTRY(SE_VX_KK),
        OPCODE_ID_STRING_ENTRY(SNE_VX_KK),
        OPCODE_ID_STRING_ENTRY(SE_VX_VY),
        OPCODE_ID_STRING_ENTRY(LD_VX_KK),
        OPCODE_ID_STRING_ENTRY(ADD_VX_KK),
        OPCODE_ID_STRING_ENTRY(LD_VX_VY),
        OPCODE_ID_STRING_ENTRY(OR_VX_VY),
        OPCODE_ID_STRING_ENTRY(AND_VX_VY),
        OPCODE_ID_STRING_ENTRY(XOR_VX_VY),
        OPCODE_ID_STRING_ENTRY(ADD_VX_VY),
        OPCODE_ID_STRING_ENTRY(SUB_VX_VY),
        OPCODE_ID_STRING_ENTRY(SHR_VX_VY),
        OPCODE_ID_STRING_ENTRY(SUBN_VX_VY),
        OPCODE_ID_STRING_ENTRY(SHL_VX_VY),
        OPCODE_ID_STRING_ENTRY(SNE_VX_VY),
        OPCODE_ID_STRING_ENTRY(LD_I_ADDR),
        OPCODE_ID_STRING_ENTRY(JP_V0_ADDR),
        OPCODE_ID_STRING_ENTRY(RND_VX_KK),
        OPCODE_ID_STRING_ENTRY(DRW_VX_VY_N),
        OPCODE_ID_STRING_ENTRY(SKP_VX),
        OPCODE_ID_STRING_ENTRY(SKNP_VX),
        OPCODE_ID_STRING_ENTRY(LD_VX_DT),
        OPCODE_ID_STRING_ENTRY(LD_VX_K),
        OPCODE_ID_STRING_ENTRY(LD_DT_VX),
        OPCODE_ID_STRING_ENTRY(LD_ST_VX),
        OPCODE_ID_STRING_ENTRY(ADD_I_VX),
        OPCODE_ID_STRING_ENTRY(LD_F_VX),
        OPCODE_ID_STRING_ENTRY(LD_B_VX),
        OPCODE_ID_STRING_ENTRY(LD_I_VX),
        OPCODE_ID_STRING_ENTRY(LD_VX_I)
    };

    auto it = map.find(id);
    return it != map.end() ? it->second : "UNKNOWN";
}
