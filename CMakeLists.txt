cmake_minimum_required(VERSION 3.15)
project(ScriptHookRDR-Example)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/inc)

# Add subdirectories
add_subdirectory(src)