#pragma once
//--------------------------------------------------------------------------------
// Emulator
#include "Common.h"

// System
#include <string>
#include <vector>

//--------------------------------------------------------------------------------
class RomManager
{
public:
	explicit RomManager(const fs::path& romsRoot);
	
	fs::path ResolveRom(const std::string& relativePath) const;

private:
	fs::path mRomsRoot;
};