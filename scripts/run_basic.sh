#!/bin/bash

# Get the current working directory (basepath)
BASE_PATH=$(pwd)

# Define optimization levels
OPT_LEVELS=("O0" "O1" "O2" "O3")

# Loop over the optimization levels and run the existing executables
for OPT in "${OPT_LEVELS[@]}"; do
    # Define the directory where the executable is located
    EXEC_DIR="${BASE_PATH}/C/DGEMM_basic_${OPT}"
    
    # Define the executable path
    EXEC_PATH="${EXEC_DIR}/DGEMM_${OPT}"

    # Check if the executable exists before running it
    if [ -f "$EXEC_PATH" ]; then
        # Change to the directory where the executable is located
        cd "$EXEC_DIR"
        
        # Run the compiled program in the background using 'nohup' and redirect output to the correct log file
        nohup "./DGEMM_${OPT}" > "output.log" 2>&1 &
        
        # Optionally, show that the program has been started in the background
        echo "Started DGEMM_${OPT} in the background, output logged to ${EXEC_DIR}/output.log"
    else
        # If the executable does not exist, print an error message
        echo "Error: Executable $EXEC_PATH does not exist in ${EXEC_DIR}."
    fi
done

# Wait for all background processes to finish before exiting the script
wait

echo "All programs are running in the background."

