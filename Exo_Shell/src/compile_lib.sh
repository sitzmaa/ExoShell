#!/bin/bash

# Set the directories
LIB_DIR="../lib"
BIN_DIR="$HOME/exo_bin"

# Check if the exo_bin directory exists in the home directory; if not, create it
if [ ! -d "$BIN_DIR" ]; then
  mkdir "$BIN_DIR"
  echo "Created $BIN_DIR directory."
fi

# Loop through all .cpp files in the lib directory
for file in "$LIB_DIR"/*.cpp; do
  # Get the base filename without the directory or extension
  filename=$(basename -- "$file")
  name="${filename%.*}"

  # Compile the C++ file into the ~/exo_bin directory
  g++ "$file" -std=c++17 -o "$BIN_DIR/$name"

  if [ $? -eq 0 ]; then
    echo "Compiled $file -> $BIN_DIR/$name"
  else
    echo "Error compiling $file"
  fi
done
