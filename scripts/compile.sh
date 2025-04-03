#!/bin/bash

# Get the current working directory (basepath)
BASE_PATH=$(pwd)

# Define the input source file relative to the current directory
SOURCE_FILE="${BASE_PATH}/raw_code/DGEMM_basic.c"

# Define optimization levels
OPT_LEVELS=("O0" "O1" "O2" "O3")

# Loop over the optimization levels and compile
for OPT in "${OPT_LEVELS[@]}"; do
    # Create a directory for each optimization level in the basepath
    OUTPUT_PATH="${BASE_PATH}/C/DGEMM_basic_${OPT}"
    mkdir -p "$OUTPUT_PATH"
    
    # Compile the source file with the current optimization level
    gcc -${OPT} "$SOURCE_FILE" -o "${OUTPUT_PATH}/DGEMM_${OPT}"
    
    echo "Compiled with -O${OPT} and output to ${OUTPUT_PATH}/DGEMM_${OPT}"
done

echo "Compilation completed with all optimization levels."

