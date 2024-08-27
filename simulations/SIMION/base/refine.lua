-- Import all helper functions
simion.import("file_io.lua")
simion.import("geometry.lua")
simion.import("electrode_potentials.lua")
simion.import("ions.lua")

-- Adjust geometry parameters from defaults for conversion
-- and to keep memory usage low
-- set_electrode_pair_count(220)
set_grid_x_spacing(2E-3)
set_grid_y_spacing(1E-3)
set_grid_z_spacing(1E-2)
set_device_z_length(0)
set_simulated_z_device_length(3)

-- Generate HALF-IMS potential arrays from geometry file
simion.command("--noprompt gem2pa HALF-IMS.gem HALF-IMS.pa#")

-- Refine geometry
simion.command("--noprompt refine HALF-IMS.pa#")

os.remove("HALF-IMS.iob")

print("\n\n------------------------------------------------------------------")
print("Before running the 'simulate.lua' program, please load the generated PA, \nopen a workbench with the refined PA and save as 'HALF-IMS.iob' to \ncomply with the SIMION workflow.")