#include "Utils.h"

// Includes
//--------------------------------------------------------------------------------
// System
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>

//--------------------------------------------------------------------------------
void BinaryToHex(const std::string& inputFileName, const std::string& outputFileName)
{
    std::ifstream inputFile(inputFileName, std::ios::binary);
    std::ofstream outputFile(outputFileName);

    if (!inputFile.is_open())
    {
        std::cerr << "Could not open the input file." << std::endl;
        return;
    }

    if (!outputFile.is_open()) {
        std::cerr << "Could not open the output file." << std::endl;
        return;
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    size_t totalBytes = buffer.size();

    for (size_t i = 0; i < totalBytes; i += 8)
    {
        for (size_t j = 0; j < 8 && (i + j) < totalBytes; j += 2)
        {
            if (j > 0)
            {
                outputFile << " ";
            }

            outputFile << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(static_cast<uint8_t>(buffer[i + j]));
            outputFile << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(static_cast<uint8_t>(buffer[i + j + 1]));
        }
        outputFile << std::endl;
    }

    inputFile.close();
    outputFile.close();
}