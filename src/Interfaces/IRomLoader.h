#pragma once

// Includes
//--------------------------------------------------------------------------------
// System
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

//--------------------------------------------------------------------------------
class IRomLoader
{
public:
	virtual ~IRomLoader() = default;

	virtual size_t RomCount() const = 0;
	virtual const std::vector<std::string>& GetRoms() const = 0;
	virtual std::vector<uint8_t> LoadRom(const std::string& romName) const = 0;
};