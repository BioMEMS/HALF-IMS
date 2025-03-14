--This program assumes the user has run the refine.lua and then created an ion workbench file with the name "HALF-IMS.iob"

-- Import all helper functions
simion.import("file_io.lua")
simion.import("geometry.lua")
simion.import("electrode_potentials.lua")
simion.import("ions.lua")

-- Initialize ion library definitions
initialize_ion_library()

local grouped_flag = get_iob_grouped_setting()
local repulsion_type = get_iob_grouped_repulsion()
local repulsion_value = get_iob_grouped_repulsion_value()

-- Set image control to View On, Retain Off
set_iob_trajectory_image_control(1)

-- For each file in the list
for file in string.gmatch(get_ion_files(),"([^,]+)") do
    for rate=min_gas,max_gas,step_gas do
	--Set carrier gas flow rate
	set_carrier_gas_rate(rate)

	--Set the current ion file for logging
	set_current_ion_file(file)

	if (grouped_flag == 1) then
	   if (repulsion_type == "none") then
	      simion.command("--noprompt fly HALF-IMS.iob --particles=" .. get_current_ion_file() .. " --grouped=" .. tostring(grouped_flag) .. " --repulsion=" .. repulsion_type)
	   else
	      simion.command("--noprompt fly HALF-IMS.iob --particles=" .. get_current_ion_file() .. " --grouped=" .. tostring(grouped_flag) .. " --repulsion=" .. repulsion_type .. " --repulsion-amount=" .. tostring(repulsion_value))
	   end
	 else
	    simion.command("--noprompt fly HALF-IMS.iob --particles=" .. get_current_ion_file())	
	 end
    end	    	   
end

-- Clean up simulation
set_results_file_name("output.csv")
set_short_electrode_potential(0)
set_long_electrode_potential(0)
set_bias_ring_electrode_potential(5)
set_iob_trajectory_image_control(0)
