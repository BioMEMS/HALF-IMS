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

local min_gas = get_carrier_gas_min_flow_rate()
local max_gas = get_carrier_gas_max_flow_rate()
local step_gas = get_carrier_gas_step_flow_rate()

set_results_file_name("output_" .. tostring(os.time()) .. ".csv")

-- For each file in the list
for file in string.gmatch(get_results_files(),"([^,]+)") do

    --Set the current ion file for logging
    set_current_ion_file(file)
    
    for rate=min_gas,max_gas,step_gas do
    	--Set carrier gas flow rate
	set_carrier_gas_rate(rate)

    	for lep=min_long,max_long,step_long do
	    --Set long electrode voltage
	    set_long_electrode_potential(lep)

	    for sep=min_short,max_short,step_short do
		-- Set short electrode voltage
		set_short_electrode_potential(sep)

		simion.command("--noprompt fly HALF-IMS.iob --particles=" .. file)
	     end
	end
    end
end

set_short_electrode_potential(0)
set_long_electrode_potential(0)