# List available recipes
default:
    @just --list

# Build the plugin in Release mode
build:
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build

# Format source code using clang-format
format:
    find source -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Lint source code using clang-tidy
lint:
    cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    clang-tidy -p build source/*.cpp

# Clean build artifacts
clean:
    rm -rf build
