#pragma once

// Includes
//--------------------------------------------------------------------------------
// System
#include "Interfaces/IRomLoader.h"

// System
#include <vector>
#include <string>
#include <string_view>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cassert>

// Namespace Alias
//--------------------------------------------------------------------------------
namespace fs = std::filesystem;

//--------------------------------------------------------------------------------
class RomLoader : public IRomLoader
{
public:
    explicit RomLoader(const fs::path& romsRoot)
        : mRomsRoot(romsRoot)
    {
        LoadRomsFromDirectory();
    }

    size_t RomCount() const override
    {
        return mRoms.size();
    }

    const std::vector<std::string>& GetRoms() const override
    {
        return mRoms;
    }

    std::vector<uint8_t> LoadRom(std::string_view romName) const override
    {
        fs::path romPath = mRomsRoot / std::string(romName);
        std::ifstream file(romPath, std::ios::binary);
        if (!file)
        {
            std::cerr << "Failed to open ROM: " << romPath << std::endl;
            return {};
        }

        file.seekg(0, std::ios::end);
        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        if (fileSize <= 0)
        {
            std::cerr << "ROM is empty: " << romPath << std::endl;
            return {};
        }

        std::vector<uint8_t> buffer(static_cast<size_t>(fileSize));
        if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
        {
            std::cerr << "Failed to read full ROM: " << romPath << std::endl;
            return {};
        }

        return buffer;
    }

private:
    void LoadRomsFromDirectory()
    {
        assert(mRoms.empty());

        if (!fs::exists(mRomsRoot) || !fs::is_directory(mRomsRoot))
        {
            std::cerr << "Invalid ROM directory: " << mRomsRoot << '\n';
            return;
        }

        for (const auto& entry : fs::directory_iterator(mRomsRoot))
        {
            if (fs::is_regular_file(entry.status()))
            {
                mRoms.push_back(entry.path().filename().string());
            }
        }
    }

    fs::path mRomsRoot;
    std::vector<std::string> mRoms;
};