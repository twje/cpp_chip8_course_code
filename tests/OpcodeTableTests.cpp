#include <gtest/gtest.h>
#define UNIT_TESTING

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interpreter/Instruction/OpcodeTable.h"
#include "Interpreter/Instruction/OpcodeId.h"

// System
#include <vector>
#include <unordered_set>

// Ensure that each opcode pattern in the OpcodeTable is uniquely decodable.
// Only SYS (0nnn) may overlap with CLS (00E0) and RET (00EE), which are stricter matches.
//--------------------------------------------------------------------------------
TEST(OpcodeTableTests, NoUnexpectedDecodeOverlaps)
{
    // Known overlaps are intentional: SYS (0nnn) may match CLS (00E0) or RET (00EE)
    const std::vector<OpcodeId> allowedSYSOverlaps{ OpcodeId::CLS, OpcodeId::RET };

    auto IsAllowedSYSOverlap = [&](OpcodeId a, OpcodeId b) {
        return (a == OpcodeId::SYS_ADDR &&
            std::find(allowedSYSOverlaps.begin(), allowedSYSOverlaps.end(), b) != allowedSYSOverlaps.end());
    };

    auto DecodersMayOverlap = [](const OpcodeSpec& a, const OpcodeSpec& b) {
        // This returns true if there's no bit conflict in the shared masked bits.
        return ((a.mPattern ^ b.mPattern) & (a.mMask & b.mMask)) == 0;
    };

    const auto& table = OpcodeTable::All();

    for (size_t i = 0; i < table.size(); ++i)
    {
        const auto& specA = table[i];

        for (size_t j = i + 1; j < table.size(); ++j)
        {
            const auto& specB = table[j];

            // Allow SYS to overlap with CLS or RET, but no other overlaps are valid.
            if (IsAllowedSYSOverlap(specA.mOpcodeId, specB.mOpcodeId) ||
                IsAllowedSYSOverlap(specB.mOpcodeId, specA.mOpcodeId))
            {
                continue;
            }

            if (DecodersMayOverlap(specA, specB))
            {
                FAIL() << "Unexpected overlap between: "
                    << specA.mMnemonic << " and " << specB.mMnemonic;
            }
        }
    }
}

//--------------------------------------------------------------------------------
TEST(OpcodeTableTests, UniqueOpcodeIds)
{
    std::unordered_set<OpcodeId> seen;

    for (const OpcodeSpec& spec : OpcodeTable::All())
    {
        ASSERT_TRUE(seen.insert(spec.mOpcodeId).second)
            << "Duplicate OpcodeId found: " << spec.mMnemonic;
    }
}

//--------------------------------------------------------------------------------
TEST(OpcodeTableTests, PatternWithinMask)
{
    for (const OpcodeSpec& spec : OpcodeTable::All())
    {
        ASSERT_EQ(spec.mPattern & spec.mMask, spec.mPattern)
            << "Pattern has bits set outside mask: " << spec.mMnemonic;
    }
}