-- Import all helper functions
simion.import("file_io.lua")
simion.import("geometry.lua")
simion.import("electrode_potentials.lua")

-- Adjust geometry parameters from defaults for conversion
-- and to keep memory usage low
--set_electrode_pair_count(220)
set_grid_x_spacing(2E-6)
set_grid_y_spacing(1E-6)
set_grid_z_spacing(1E-5)
set_device_z_length(0)

-- Generate HALF-IMS potential arrays from geometry file
simion.command("--noprompt gem2pa HALF-IMS.gem HALF-IMS.pa#")

-- Refine geometry
simion.command("--noprompt refine HALF-IMS.pa#")

print("\n\n------------------------------------------------------------------")
print("Before running the 'simulate.lua' program, please load the generated PA, \nopen a workbench with the refined PA and save as 'HALF-IMS.iob' to \ncomply with the SIMION workflow.")