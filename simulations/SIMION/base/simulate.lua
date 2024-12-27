--This program assumes the user has run the refine.lua and then created an ion workbench file with the name "HALF-IMS.iob"

-- Import all helper functions
simion.import("file_io.lua")
simion.import("geometry.lua")
simion.import("electrode_potentials.lua")
simion.import("ions.lua")

-- Initialize ion library definitions
initialize_ion_library()

local min_long = get_long_electrode_min_potential()
local max_long = get_long_electrode_max_potential()
local step_long = get_long_electrode_step_potential()

local min_short = get_short_electrode_min_potential()
local max_short = get_short_electrode_max_potential()
local step_short = get_short_electrode_step_potential()

local min_bias = get_bias_electrode_min_potential()
local max_bias = get_bias_electrode_max_potential()
local step_bias = get_bias_electrode_step_potential()

local step_gas = get_carrier_gas_step_flow_rate()
local min_gas = get_carrier_gas_min_flow_rate()
local max_gas = get_carrier_gas_max_flow_rate()

local repetition_count = get_simulate_loop_count()
local grouped_flag = get_iob_grouped_setting()
local repulsion_type = get_iob_grouped_repulsion()
local repulsion_value = get_iob_grouped_repulsion_value()

-- Rely on Simulate GUI to create output file name
--set_results_file_name("output_" .. tostring(os.time()) .. ".csv")

-- For all desired repetitions
for repetition=0,repetition_count,1 do 

    -- For each file in the list
    for file in string.gmatch(get_ion_files(),"([^,]+)") do

	for bep=min_bias,max_bias,step_bias do
	    --Set bias ring voltage
	    set_bias_ring_electrode_potential(bep)

	    for rate=min_gas,max_gas,step_gas do
		--Set carrier gas flow rate
		set_carrier_gas_rate(rate)

		--Set the current ion file for logging
		set_current_ion_file(file)

		for lep=min_long,max_long,step_long do
		    --Set long electrode voltage
		    set_long_electrode_potential(lep)

		    for sep=min_short,max_short,step_short do
			-- Set short electrode voltage
			set_short_electrode_potential(sep)

			if(grouped_flag == 1) then
			   if(repulsion_type == "none") then
			      simion.command("--noprompt fly HALF-IMS.iob --particles=" .. get_current_ion_file() .. " --grouped=" .. tostring(grouped_flag) .. " --repulsion=" .. repulsion_type)
			   else
			      simion.command("--noprompt fly HALF-IMS.iob --particles=" .. get_current_ion_file() .. " --grouped=" .. tostring(grouped_flag) .. " --repulsion=" .. repulsion_type .. " --repulsion-amount=" .. tostring(repulsion_value))
			   end
			else
			   simion.command("--noprompt fly HALF-IMS.iob --particles=" .. get_current_ion_file())	
			end
		     end
		end
	    end
	end    
    end
end

-- Clean up simulation
set_results_file_name("output.csv")
set_short_electrode_potential(0)
set_long_electrode_potential(0)
set_bias_ring_electrode_potential(0.01)