#!/bin/bash

# Define the directory where CPLEX generates the files
TARGET_DIR="/home/ana/Documents/PHD/Research/Implementation/sarpfiles"  # Change this to the directory where the bt files are generated

# Navigate to the target directory
cd "$TARGET_DIR" || exit

# Remove all bt-*.txt files
rm -f bt-*.txt

# Confirm deletion
echo "Removed all bt-*.txt files from $TARGET_DIR"
