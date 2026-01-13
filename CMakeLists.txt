cmake_minimum_required(VERSION 3.16)
project(Pix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Compile Commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Add subdirectories
add_subdirectory(X)
add_subdirectory(Pix)
