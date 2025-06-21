#include "Instruction.h"

// Includes
//--------------------------------------------------------------------------------
// System
#include <iomanip>

//--------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Instruction& instruction)
{
	// TODO: fix
	/*
	os << std::hex << std::uppercase;

	// Print address with zero-padding
	os << "Address: 0x" << std::setw(4) << std::setfill('0') << instr.mAddress;

	// Reset padding to space for other fields
	os << std::setfill(' ') << " | ";

	// Print opcode ID, left-aligned and padded
	os << "Opcode: " << std::left << std::setw(12) << ToString(instr.mOpcodePatternId) << " | ";

	// Print arguments
	os << "Args: ";
	if (instr.mArguments.empty())
	{
		os << "(none)";
	}
	else
	{
		for (size_t i = 0; i < instr.mArguments.size(); ++i)
		{
			os << "0x" << std::setw(2) << std::setfill('0') << instr.mArguments[i];
			if (i < instr.mArguments.size() - 1)
			{
				os << ", ";
			}
		}
		os << std::setfill(' '); // Reset after args
	}
	*/

	return os;
}