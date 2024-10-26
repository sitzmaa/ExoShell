#!/bin/bash

# Set the directories
LIB_DIR="../lib"
SRC_DIR="."

# Check if the lib directory exists
if [ ! -d "$LIB_DIR" ]; then
  echo "Error: Directory $LIB_DIR does not exist."
  exit 1
fi

# Loop through all .cpp files in the lib directory
for file in "$LIB_DIR"/*.cpp; do
  # Get the base filename without the directory or extension
  filename=$(basename -- "$file")
  name="${filename%.*}"

  # Compile the C++ file into the src directory
  g++ "$file" -std=c++17 -o "$SRC_DIR/$name"

  if [ $? -eq 0 ]; then
    echo "Compiled $file -> $SRC_DIR/$name"
  else
    echo "Error compiling $file"
  fi
done
