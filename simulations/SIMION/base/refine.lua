-- Import all helper functions
simion.import("file_io.lua")
simion.import("geometry.lua")

-- Adjust geometry parameters from defaults for conversion
-- and to keep memory usage low
set_electrode_pair_count(25)
set_grid_x_spacing(2E-6)
set_grid_y_spacing(1E-6)
set_grid_z_spacing(1E-5)

-- Generate HALF-IMS potential arrays from geometry file
simion.command("--noprompt gem2pa HALF-IMS.gem HALF-IMS.pa#")

-- Refine geometry
simion.command("--noprompt refine HALF-IMS.pa#")

-- Adjust user parameters for flying...

--simion.command("--noprompt fly HALF-IMS.iob")