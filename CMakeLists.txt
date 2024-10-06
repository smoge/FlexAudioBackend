cmake_minimum_required(VERSION 3.12)
project(AudioBackendProject VERSION 1.0.0 LANGUAGES CXX)

# C++ standard configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# Options
option(BUILD_EXAMPLES "Build example projects" ON)
option(BUILD_TESTS "Build test projects" OFF)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(JACK REQUIRED IMPORTED_TARGET jack)
pkg_check_modules(PIPEWIRE REQUIRED IMPORTED_TARGET libpipewire-0.3)

message(STATUS "PipeWire include directories: ${PIPEWIRE_INCLUDE_DIRS}")
message(STATUS "PipeWire libraries: ${PIPEWIRE_LIBRARIES}")

# Library target
add_library(audio_backend
    src/audio_backend.cpp
    src/jack_backend.cpp
    src/pipewire_backend.cpp
)

# Include directories and link libraries for audio_backend target
target_include_directories(audio_backend PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${JACK_INCLUDE_DIRS}
    ${PIPEWIRE_INCLUDE_DIRS}
)


target_link_libraries(audio_backend PRIVATE
    PkgConfig::JACK
    PkgConfig::PIPEWIRE
)

# Install rules
install(TARGETS audio_backend
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include/audio_backend
    FILES_MATCHING PATTERN "*.h"
)

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples/simple_client)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Print build configuration
message(STATUS "Build configuration:")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
