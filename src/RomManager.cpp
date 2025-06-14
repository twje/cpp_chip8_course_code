#include "RomManager.h"

//--------------------------------------------------------------------------------
RomManager::RomManager(const fs::path& romsRoot)
	: mRomsRoot(romsRoot)
{ }

//--------------------------------------------------------------------------------
fs::path RomManager::ResolveRom(const std::string& relativePath) const
{
	return mRomsRoot / relativePath;
}