#pragma once

//--------------------------------------------------------------------------------
class Instruction
{
public:
    Instruction(OpcodeId id, std::vector<uint16_t>&& arguments)
        : mId(id)
        , mArguments(std::move(arguments))
    { }

    OpcodeId GetId() const { return mId; }
    const std::vector<uint16_t>& GetArguments() const { return mArguments; }

private:
    OpcodeId mId;
    std::vector<uint16_t> mArguments;
};
