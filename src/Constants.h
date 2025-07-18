#pragma once

// Includes
//------------------------------------------------------------------------------
// System
#include <array>
#include <filesystem>

// Namespace Alias
//------------------------------------------------------------------------------
namespace fs = std::filesystem;

// Constants
//--------------------------------------------------------------------------------
// Misc
inline constexpr uint16_t kFontSpriteSize = 5;

// Screen config
inline constexpr int32_t SCREEN_WIDTH = 452;
inline constexpr int32_t SCREEN_HEIGHT = 323;
inline constexpr int32_t SCREEN_PIXEL_SCALE = 2;

// Memory config
inline constexpr uint16_t RAM_SIZE = 4096;
inline constexpr uint16_t PROGRAM_START_ADDRESS = 0x200;
inline constexpr uint8_t STACK_SIZE = 16;
inline constexpr uint8_t REGISTER_COUNT = 16;

// Display config
inline constexpr uint8_t DISPLAY_WIDTH = 64;
inline constexpr uint8_t DISPLAY_HEIGHT = 32;
inline constexpr uint16_t DISPLAY_PIXEL_COUNT = DISPLAY_WIDTH * DISPLAY_HEIGHT;
inline constexpr int32_t UI_CHAR_SIZE = 8;

// Timer config
inline constexpr float CPU_FREQUENCY_HZ = 500.0f;
inline constexpr float SYSTEM_TIMER_HZ = 60.0f;

// Input config
inline constexpr uint8_t NUM_KEYS = 16;

// Instruction config
inline constexpr uint8_t INSTRUCTION_SIZE = 2;

// Font set: Hex digits 0–F
//--------------------------------------------------------------------------------
inline constexpr std::array<uint8_t, 80> CHAR_SET = {
    0xf0, 0x90, 0x90, 0x90, 0xf0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xf0, 0x10, 0xf0, 0x80, 0xf0, // 2
    0xf0, 0x10, 0xf0, 0x10, 0xf0, // 3
    0x90, 0x90, 0xf0, 0x10, 0x10, // 4
    0xf0, 0x80, 0xf0, 0x10, 0xf0, // 5
    0xf0, 0x80, 0xf0, 0x90, 0xf0, // 6
    0xf0, 0x10, 0x20, 0x40, 0x40, // 7
    0xf0, 0x90, 0xf0, 0x90, 0xf0, // 8
    0xf0, 0x90, 0xf0, 0x10, 0xf0, // 9
    0xf0, 0x90, 0xf0, 0x90, 0x90, // A
    0xe0, 0x90, 0xe0, 0x90, 0xe0, // B
    0xf0, 0x80, 0x80, 0x80, 0xf0, // C
    0xe0, 0x90, 0x90, 0x90, 0xe0, // D
    0xf0, 0x80, 0xf0, 0x80, 0xf0, // E
    0xf0, 0x80, 0xf0, 0x80, 0x80  // F
};