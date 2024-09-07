# ---------------------------- Project setup ----------------------------
cmake_minimum_required(VERSION 3.24)
project(QuickShare VERSION 0.1 LANGUAGES CXX)

# -------------------------- Set C++ settings ---------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------- Set CMake settings --------------------------
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(BOOST_ENABLE_CMAKE ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# ----------------------- Include directories ---------------------------
include_directories(${Boost_INCLUDE_DIRS})

# ------------------------- Project structure ---------------------------
add_subdirectory(src)
add_subdirectory(test)
