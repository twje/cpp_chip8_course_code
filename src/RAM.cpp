#include "RAM.h"

// Includes
//--------------------------------------------------------------------------------
#include <cassert>

//--------------------------------------------------------------------------------
uint8_t RAM::Read(uint16_t address) const
{
    assert(address < RAM_SIZE);
    return mData[address];
}

//--------------------------------------------------------------------------------
void RAM::Write(uint16_t address, uint8_t value)
{
    assert(address < RAM_SIZE);
    mData[address] = value;
}

//--------------------------------------------------------------------------------
bool RAM::WriteRange(size_t start, const std::vector<uint8_t>& data)
{
    if (start + data.size() > mData.size()) 
    {
        return false;
    }
    std::copy(data.begin(), data.end(), mData.begin() + start);
    return true;
}

//--------------------------------------------------------------------------------
void RAM::ClearProgramMemory()
{
    std::fill(mData.begin() + PROGRAM_START_ADDRESS, mData.end(), 0);
}