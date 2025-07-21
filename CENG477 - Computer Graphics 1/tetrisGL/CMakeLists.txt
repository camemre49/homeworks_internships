# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.10)

# Project Name and C++ Standard
project(tetrisGL VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler Definitions
add_definitions(-DGL_SILENCE_DEPRECATION -DGLM_ENABLE_EXPERIMENTAL)

# Source Files
set(SOURCES
        main.cpp
        GameState.h
        GameState.cpp
        GameConstants.h
        callbackHandler.cpp
        callbackHandler.h
)

# Find Required Packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(FREETYPE REQUIRED freetype2)

# Include Directories
include_directories(
        ${FREETYPE_INCLUDE_DIRS}
        .
)

# Link Directories
link_directories(
        ${FREETYPE_LIBRARY_DIRS}
)

# Add Executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link Libraries
target_link_libraries(${PROJECT_NAME}
        glfw
        png
        pthread
        X11
        dl
        Xrandr
        GLEW
        GL
        ${FREETYPE_LIBRARIES}
)

# Debugging Flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -g)
endif()
