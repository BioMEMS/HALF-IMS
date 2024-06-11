-- Import all helper functions
simion.import("file_io.lua")
simion.import("geometry.lua")
simion.import("electrode_potentials.lua")

-- Initialize ion library definitions
initialize_ion_library()

-- Adjust geometry parameters from defaults for conversion
-- and to keep memory usage low
set_electrode_pair_count(220)
set_grid_x_spacing(2E-6)
set_grid_y_spacing(1E-6)
set_grid_z_spacing(1E-5)
set_device_z_length(0)

-- Generate HALF-IMS potential arrays from geometry file
simion.command("--noprompt gem2pa HALF-IMS.gem HALF-IMS.pa#")

-- Refine geometry
simion.command("--noprompt refine HALF-IMS.pa#")

for lep=0,10,0.25 do
    for sep=0,60,0.25 do
    	-- Adjust user parameters for flying...
	   set_short_electrode_potential(sep)
	   set_long_electrode_potential(lep)
	   --simion.command("--noprompt fly HALF-IMS.iob")
     end
end