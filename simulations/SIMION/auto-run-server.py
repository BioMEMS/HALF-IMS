#!/usr/bin/env python3

import sys
import os
import time
import shutil
import subprocess

if (len(sys.argv) < 2 or not os.path.isdir(sys.argv[1])):
    print("Please provide a valid target directory for SIMION configuration files.")
    sys.exit(1)

# Build the target file path
target_file="config.tmp"
target_path=os.path.abspath(sys.argv[1] + target_file)
destination_path=os.path.abspath("./base/" + target_file)

# Subprocess command to run on configuration file change
command="simion.exe lua simulate.lua"

# Indicate target path for files
print("Target Path:", target_path)
print("Desination Path:", destination_path)

while True:
    # If the configuration file exists in the target path
    if(os.path.isfile(target_path)):
        # Move the newly detected configuration file
        print("New configuration file found.")
        shutil.move(target_path, destination_path)

        # Execute the SIMION command with the new configuration file
        print("Running new configuration file.")
        subprocess.run([command, destination_path])
        
    
    

