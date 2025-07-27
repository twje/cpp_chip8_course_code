#pragma once

// Includes
//--------------------------------------------------------------------------------
// System
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

//--------------------------------------------------------------------------------
class IRomLoader
{
public:
	virtual ~IRomLoader() = default;

	virtual size_t RomCount() const = 0;
	virtual const std::vector<std::string>& GetRoms() const = 0;
	virtual std::vector<uint8_t> LoadRom(std::string_view romName) const = 0;
};