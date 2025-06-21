cmake_minimum_required(VERSION 3.20)
project(Chip8Emulator VERSION 0.1.0 LANGUAGES CXX)

#-------------------------------------------------------------------------------
# Compiler Configuration
#-------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PRODUCTION_BUILD "Make this a production build" OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -pedantic -Wconversion)
endif()

#-------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------

include(FetchContent)

# GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG        v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#-------------------------------------------------------------------------------
# Vendor Includes
#-------------------------------------------------------------------------------

# Suppress warnings for olcPixelGameEngine
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/vendor/PGE)

#-------------------------------------------------------------------------------
# Source and Core Library
#-------------------------------------------------------------------------------

# Gather only core source files (excluding main.cpp)
file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS src/core/*.cpp src/core/*.h)

# Build core logic as a static library
add_library(Chip8Core STATIC ${CORE_SRC})
target_include_directories(Chip8Core PUBLIC src/core)

# Optional: define ROMS_PATH for dev builds
if(NOT PRODUCTION_BUILD)
    target_compile_definitions(Chip8Core PUBLIC ROMS_PATH="${PROJECT_SOURCE_DIR}/roms/")
endif()

#-------------------------------------------------------------------------------
# Application Executable
#-------------------------------------------------------------------------------

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Chip8Core)

#-------------------------------------------------------------------------------
# Unit Tests
#-------------------------------------------------------------------------------

file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS tests/*.cpp)

if(TEST_FILES)
    add_executable(${PROJECT_NAME}_tests ${TEST_FILES})
    target_link_libraries(${PROJECT_NAME}_tests PRIVATE Chip8Core gtest_main)

    include(CTest)
    enable_testing()
    add_test(NAME AllTests COMMAND ${PROJECT_NAME}_tests)
endif()
