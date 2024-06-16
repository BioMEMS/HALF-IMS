simion.import("file_io.lua")
simion.import("geometry.lua")

--Functions to set/get a list of ion files to process
local simulation_ions_file_name = "simulate_files_list"
function get_results_files()
	 return get_raw_file_value(simulation_ions_file_name, "")
end

function set_results_files(value_list)
	 -- Write generated string to the file
	 set_file_value(simulation_ions_file_name, value_list)
	 return
end

--Functions to set/get the current ion file
local simulation_current_ion_file_name = "simulate_current_ion_file"
function get_current_ion_file()
	 return get_raw_file_value(simulation_current_ion_file_name, "NONE")
end

function set_current_ion_file(file_name)
	 local temp = ""

	 -- Split the input 
	 for name in string.gmatch(file_name,"([^\\]+)") do
	     -- Assign each segment to eventually get the file name itself
	     temp = name
	 end
	 
	 set_file_value(simulation_current_ion_file_name, temp)
end

--Functions to get/set the X-axis velocity of the new ions
local ion_initial_x_velocity_file_name = "ion_init_x_velocity"

function get_ion_x_velocity()
	 return get_file_value(ion_initial_x_velocity_file_name, 1)
end

function set_ion_x_velocity(value)
	 set_file_value(ion_initial_x_velocity_file_name, value)
	 return
end

--Functions to get/set the Y-axis velocity of the new ions
local ion_initial_y_velocity_file_name = "ion_init_y_velocity"

function get_ion_y_velocity()
	 return get_file_value(ion_initial_y_velocity_file_name, 0)
end

function set_ion_y_velocity(value)
	 set_file_value(ion_initial_y_velocity_file_name, value)
	 return
end

--Functions to get/set the Z-axis velocity of the new ions
local ion_initial_z_velocity_file_name = "ion_init_z_velocity"

function get_ion_z_velocity()
	 return get_file_value(ion_initial_z_velocity_file_name, 0)
end

function set_ion_z_velocity(value)
	 set_file_value(ion_initial_z_velocity_file_name, value)
	 return
end

--Functions to set/get simulated Z-device size
local simulated_z_device_length_file_name = "simulated_z_device_length"
function get_simulated_z_device_length()
	 return get_file_value(simulated_z_device_length_file_name, 3E-3)
end

function set_simulated_z_device_length(value)
	 set_file_value(simulated_z_device_length_file_name, value)
	 return
end

--Functions to get/set the carrier gas flow rate
local carrier_gas_flow_rate_file_name = "carrier_gas_flow_rate"

function get_carrier_gas_rate()
	 return get_file_value(carrier_gas_flow_rate_file_name, 1000)
end

-- Carrier gas provided in mL/min
function set_carrier_gas_rate(value)
	 -- Set the file value
	 set_file_value(carrier_gas_flow_rate_file_name, value)

	 local z_dimension = get_device_z_length()

	 -- If the simulation is running cross-sectionally (i.e. Z-dimension is zero)
	 if (z_dimension == 0) then
	     -- Get the simulated Z-axis dimension instead to calculate velocity properly
	     z_dimension = get_simulated_z_device_length()
	 else
		-- Otherwise, use grid units to calculate real-world area
		z_dimension = z_dimension*get_grid_z_spacing()
	 end
	 
	 -- Calculate channel area
	 local channelArea = z_dimension*(get_grid_y_spacing()*get_device_y_length())

	 -- Calculate velocity by converting area from mL/(min-m^2) to m/s
	 local velocity = (value/channelArea)*(1/1E6)*(1/60)
	 
	 -- Set ion X velocity under assumption of X-axis flow
	 set_ion_x_velocity(velocity)
	 
	 return
end

--Functions to get/set the carrier gas simulation parameters
local carrier_gas_min_file_name = "carrier_gas_min_flow_rate"
local carrier_gas_max_file_name = "carrier_gas_max_flow_rate"
local carrier_gas_step_file_name = "carrier_gas_step_flow_rate"

function get_carrier_gas_min_flow_rate()
	 return get_file_value(carrier_gas_min_file_name, 10)
end

function set_carrier_gas_min_flow_rate(value)
	 set_file_value(carrier_gas_min_file_name, value)
	 return
end

function get_carrier_gas_max_flow_rate()
	 return get_file_value(carrier_gas_max_file_name, 1000)
end

function set_carrier_gas_max_flow_rate(value)
	 set_file_value(carrier_gas_max_file_name, value)
	 return
end

function get_carrier_gas_step_flow_rate()
	 return get_file_value(carrier_gas_step_file_name, 10)
end

function set_carrier_gas_step_flow_rate(value)
	 set_file_value(carrier_gas_step_file_name, value)
	 return
end
