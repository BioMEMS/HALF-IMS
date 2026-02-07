#!/usr/bin/env python3

import sys
import os
import time
import shutil
import subprocess
import re
import datetime
import glob

if (len(sys.argv) < 2 or not os.path.isdir(sys.argv[1])):
    print("Please provide a valid target directory for SIMION configuration files.")
    sys.exit(1)

# Build the target file path
target_file_extension=".config"
completed_file_extension=".completed"
target_file="settings" + target_file_extension
target_path=os.path.abspath(sys.argv[1] + "/*" + target_file_extension)
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
    target_file_list = glob.glob(target_path)
    
    # If the configuration file exists in the target path
    if(len(target_file_list) > 0):
        time.sleep(10)
        # Copy the newly detected configuration file
        print("New configuration file found.")
        shutil.copyfile(target_file_list[0], destination_path)

        # Execute the SIMION command with the new configuration file
        print("Running new configuration file.")
        subprocess.run([command], shell=True)

        # Move any detected CSV files back to target directory
        print("Moving results to configuration file directory.")
        for item in os.listdir(simulation_directory_path):
            if(len(re.findall(".*\\.csv$",item)) > 0):
                print("Moving", item)
                shutil.move(os.path.abspath(simulation_directory_path + "/" + item), os.path.abspath(sys.argv[1]))
                
        # Once finished, change the file extension to prevent re-runs while preserving a log of what was run
        print("Removing configuration file from runtime list.")
        shutil.move(target_file_list[0], target_file_list[0] + completed_file_extension)

                

