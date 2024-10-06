
# Minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Project name
project(SimpleClientExample LANGUAGES CXX)

# Add the executable target for main.cpp
add_executable(simple_client main.cpp)

# Link the audio_backend library to this executable
target_link_libraries(simple_client PRIVATE audio_backend)

# Include directories necessary for simple_client
target_include_directories(simple_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include   
)
