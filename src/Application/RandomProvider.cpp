#include "Application/RandomProvider.h"

// Includes
//--------------------------------------------------------------------------------
// System
#include <random>

// Anonymouse namespace - limits linkage to this translation unit
//--------------------------------------------------------------------------------
namespace
{
    // Static engine seeded once
    std::mt19937& GetEngine()
    {
        static std::mt19937 engine(std::random_device{}());
        return engine;
    }
}

//--------------------------------------------------------------------------------
/*virtual*/ uint8_t RandomProvider::GetRandomByte()
{
    std::uniform_int_distribution<int32_t> dist(0, 255);
    return static_cast<uint8_t>(dist(GetEngine()));
}