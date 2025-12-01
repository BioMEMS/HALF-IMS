#!/usr/bin/env python3

import sys
import os
import time
import shutil
import subprocess
import re
import datetime

if (len(sys.argv) < 2 or not os.path.isdir(sys.argv[1])):
    print("Please provide a valid target directory for SIMION configuration files.")
    sys.exit(1)

# Build the target file path
target_file="settings.config"
target_path=os.path.abspath(sys.argv[1] + "/" + target_file)
simulation_directory_path=os.path.abspath(os.path.dirname(__file__) + "/base/")
destination_path=os.path.abspath(simulation_directory_path + "/" + target_file)

# Indicate target path for files
print("Target Path:", target_path)
print("Simulation Path:", simulation_directory_path)
print("Desination Path:", destination_path)

# Subprocess command to run on configuration file change
command="simulate_automated.bat"

# Indicate command string for user
print("Command:", command)

# Change working directory
os.chdir(simulation_directory_path);

while True:
    time.sleep(30)
    print("Timestamp:",datetime.datetime.now())
    # If the configuration file exists in the target path
    if(os.path.isfile(target_path)):
        time.sleep(10)
        # Move the newly detected configuration file
        print("New configuration file found.")
        shutil.move(target_path, destination_path)

        # Execute the SIMION command with the new configuration file
        print("Running new configuration file.")
        subprocess.run([command], shell=True)

        # Move any detected CSV files back to target directory
        print("Moving results to configuration file directory.")
        for item in os.listdir(simulation_directory_path):
            if(len(re.findall(".*\\.csv$",item)) > 0):
                print("Moving", item)
                shutil.move(os.path.abspath(simulation_directory_path + "/" + item), os.path.abspath(sys.argv[1]))
                
                
                

