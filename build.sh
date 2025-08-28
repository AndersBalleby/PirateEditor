#!/bin/bash

warnings="-Wall -Wextra -Wno-writable-strings"
std="-std=c++20"

# Paths
SDL_DIR="external/SDL"
SDL_IMAGE_DIR="external/SDL_image"
SDL_TTF_DIR="external/SDL_ttf"

SDL_BUILD="$SDL_DIR/build"
SDL_IMAGE_BUILD="$SDL_IMAGE_DIR/build"
SDL_TTF_BUILD="$SDL_TTF_DIR/build"

includes="-I$SDL_DIR/include -I$SDL_IMAGE_DIR/include -I$SDL_TTF_DIR/include"

system=$(uname)

if [[ "$system" == "Linux" ]]; then
  libext="so"
elif [[ "$system" == "Darwin" ]]; then
  libext="dylib"
else
  echo "Error: Platform not supported: $system"
  exit 1
fi

libs="$SDL_BUILD/libSDL3.$libext $SDL_IMAGE_BUILD/libSDL3_image.$libext $SDL_TTF_BUILD/libSDL3_ttf.$libext"

outputFile=PirateEditor

# --- Build SDL3 ---
if [ ! -d "$SDL_BUILD" ]; then
  cmake -S $SDL_DIR -B $SDL_BUILD
fi
cmake --build $SDL_BUILD -j

# --- Build SDL_image ---
if [ ! -d "$SDL_IMAGE_BUILD" ]; then
  cmake -S $SDL_IMAGE_DIR -B $SDL_IMAGE_BUILD -DSDL3_DIR=$SDL_BUILD
fi
cmake --build $SDL_IMAGE_BUILD -j

# --- Build SDL_ttf ---
if [ ! -d "$SDL_TTF_BUILD" ]; then
  cmake -S $SDL_TTF_DIR -B $SDL_TTF_BUILD -DSDL3_DIR=$SDL_BUILD
fi
cmake --build $SDL_TTF_BUILD -j

# Collect all cpp files
cpp_files=$(find src -name "*.cpp")

# Build your project
clang++ $std $warnings $includes $cpp_files $libs -o $outputFile

if [[ $? -eq 0 ]]; then
  echo "Build succeeded: ./$outputFile"
  echo "Running..."
  echo "-------------------------------"

  if [[ "$system" == "Linux" ]]; then
    LD_LIBRARY_PATH=$SDL_BUILD:$SDL_IMAGE_BUILD:$SDL_TTF_BUILD ./$outputFile
  elif [[ "$system" == "Darwin" ]]; then
    DYLD_LIBRARY_PATH=$SDL_BUILD:$SDL_IMAGE_BUILD:$SDL_TTF_BUILD ./$outputFile
  fi
else
  echo "Build failed"
fi

