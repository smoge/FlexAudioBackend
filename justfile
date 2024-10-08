# Set the default recipe
default: build

# Define the build directory
build-dir := "build"

# Recipe for configuring the project with CMake
configure:
    @echo "Configuring project with CMake..."
    cmake -S . -B {{build-dir}}

# Recipe for building the project
build: configure
    @echo "Building project..."
    cmake --build {{build-dir}}

# Recipe for cleaning the build directory
clean:
    @echo "Cleaning build directory..."
    rm -rf {{build-dir}}

# Recipe for running the example client pipewire backend
run-pipewire: build
    @echo "Running project..."
    ./{{build-dir}}/examples/simple_client/simple_client pipewire

# Recipe for running the example client jack backend
run-jack: build
    @echo "Running project..."
    ./{{build-dir}}/examples/simple_client/simple_client jack

# Recipe for formatting code using clang-format
format:
    @echo "Formatting source files..."
    clang-format -i src/*.cpp include/*.h examples/*/*.cpp

# Show help information
help:
    @echo "Available commands:"
    @echo "  just build     - Build the project"
    @echo "  just clean     - Clean build artifacts"
    @echo "  just format    - Format source code"
