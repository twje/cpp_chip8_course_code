cmake_minimum_required(VERSION 3.20)
project(Chip8Emulator VERSION 0.1.0 LANGUAGES CXX)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Toggle production build
option(PRODUCTION_BUILD "Make this a production build" OFF)

# Use folders in IDEs (like Visual Studio)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Enable warnings (adjust as needed for cross-platform use)
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -pedantic -Wconversion)
endif()

# Add source files (recursive if needed)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
)

# Define the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Link libraries (if needed later, e.g., SDL2 or SFML)
# target_link_libraries(${PROJECT_NAME} PRIVATE some_library)

# Resource Management
if(PRODUCTION_BUILD)
    # TODO: implement
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC ROMS_PATH="${PROJECT_SOURCE_DIR}/roms/")
endif()

# Enable testing
include(CTest)
enable_testing()
