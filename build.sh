#!/bin/bash

warnings="-Wall -Wextra -Wno-writable-strings"
std="-std=c++20"
includes="-I/usr/local/include"
libs="-L/usr/local/lib -lSDL3 -lSDL3_image -lSDL3_ttf"

if [[ "$(uname)" == "Linux" ]]; then
  echo "Running on Linux"
  outputFile=PirateEditor

  cpp_files=$(find src -name "*.cpp")

  clang++ $std $warnings $includes $cpp_files $libs -o$outputFile

  if [[ $? -eq 0 ]]; then
    echo "Build suceeded: ./$outputFile"
    echo "Running..."
    echo "-------------------------------"
    ./$outputFile
  else
    echo "Build failed"
  fi
else
  echo "Error: This platform is not supported: $(uname)"
  echo "Please use Linux instead"
fi

