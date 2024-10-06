# FlexAudioBackend

FlexAudioBackend is an experimental project. It's just for learning audio systems and software design in Linux. It does not offer anything "new".
 
The goal is to explore the possibility of creating audio applications that can work with both JACK and PipeWire without changing the core application logic.

## Current Status

This project is in its early experimental stages. It is not ready for production use and may contain bugs, incomplete features, and limitations.

## Prerequisites

- CMake (version 3.12 or higher)
- C++20 compatible compiler
- JACK and PipeWire development libraries

## Building the Project

1. Clone the repository:
   ```
   git clone https://github.com/smoge/FlexAudioBackend.git
   cd FlexAudioBackend
   ```

2. Create a build directory:
   ```
   mkdir build && cd build
   ```

3. Configure the project with CMake:
   ```
   cmake ..
   ```

4. Build the project:
   ```
   cmake --build .
   ```

## Usage

To run the example client:

```
./examples/simple_client jack     # For JACK backend
./examples/simple_client pipewire # For PipeWire backend
```
