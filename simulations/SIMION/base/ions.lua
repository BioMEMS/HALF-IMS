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
	 local tempFileName = "temp.fly2"
	 
	 -- Split the input 
	 for name in string.gmatch(file_name,"([^/]+)") do
	     -- Assign each segment to eventually get the file name itself
	     temp = name
	 end

	 -- Copy file to local directory
	 copy_file(file_name, tempFileName)

	 -- Modify file to account for current geometry
	 local electrodeHeight = 2*math.ceil(get_electrode_height())
	 local yDimension = get_device_y_length()
	 local zDimension = get_device_z_length()

	 -- Determine ion count
	 ionCount = calculate_ion_count()

	 -- Open file with read/write
	 local tempID = io.open(tempFileName,"r")
	 local fileID = io.open(temp,"w")

	 -- For every line in the file
	 for line in tempID:lines() do
	     -- If it matches the first position vector
	     if (string.match(tostring(line), " *first =.*")) then
	     	 -- Updating Y position
	     	 fileID:write("      first = vector(0, " .. tostring(electrodeHeight) .. ", " .. tostring(0.2*zDimension) .. "),")
	     elseif (string.match(tostring(line), " *last =.*")) then
	     	 -- Updating Y position end
     	     	 fileID:write("      last = vector(0, " .. tostring(yDimension - electrodeHeight) .. " , " .. tostring(0.8*zDimension) .. ")")
	     elseif (string.match(tostring(line), " *n = .*")) then
	         -- Updating ion count
		 fileID:write("    n = " .. tostring(ionCount) .. ",")
             else
	         fileID:write(line)
	     end
	     
	     -- Write newline for readability
	     fileID:write("\n")
	 end
	 
	 -- Close file handles
	 fileID:close()
	 tempID:close()
	 	 
	 -- Delete temporary file
	 os.remove(tempFileName)

	 -- Set file name in configuration
	 set_file_value(simulation_current_ion_file_name, temp)
end

--Function to calculate the total number of ions based upon concentration
function calculate_ion_count()
	 return 50
end

--Functions to set/get the current chemical concentration
local chemical_concentration="drift_region_chemical_concentration"
function get_chemical_concentration()
	 return get_file_value(chemical_concentration, 0)
end

function set_chemical_concentration(value)
	 set_file_value(chemical_concentration, value)
	 return
end

--Function to calculate the X-axis ion acceleration from the carrier gas
function calculate_ion_acceleration(mass)
	 local area = calculate_ion_area(mass)
	 local pressure = get_carrier_gas_pressure()

	 -- Calculate acceleration due to the pressure, scaling to mm/usec^2
	 return ((area * pressure) / mass) * (1000) * (1E-12)
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
	 return get_file_value(simulated_z_device_length_file_name, 3)
end

function set_simulated_z_device_length(value)
	 set_file_value(simulated_z_device_length_file_name, value)
	 return
end

--Functions to get/set the carrier gas pressure
local carrier_gas_upstream_pressure_file_name = "carrier_gas_upstream_pressure"

function get_carrier_gas_pressure()
	 return get_file_value(carrier_gas_upstream_pressure_file_name, 137895)
end

function set_carrier_gas_pressure(value)
	 -- Accept PSI value and convert to N/m^2
	 set_file_value(carrier_gas_upstream_pressure_file_name, value * 6894.76)
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

	 -- Calculate velocity by converting area from mL/(min-m^2) to mm/us
	 local velocity = value/(channelArea * 60000)

	 print(value, z_dimension, channelArea, velocity)
	 
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

-- Functions to get/set the current ion's diameter
local current_ion_diameter_file_name = "current_ion_diameter"

function get_current_ion_area()
	 return get_file_value(current_ion_diameter_file_name, 0.50)
end

function set_current_ion_area(mass)
	 -- Open the SIMION 
	 local mDefsFile = os.open("m_defs.dat", "r")
	 local massLine = ""
	 local area = 0
	 local diameter = 0.5E-9
	 
	 -- For every line in the file
	 for line in mDefsFile:lines() do
	     -- If the mass is in the line
	     if (string.match(mass, line)) then
	     	-- Save the line for later parsing
	     	massLine = line       
	     end
	 end

	 -- If unable to find the ion in the m_defs file
	 if (massLine == "") then
	    print("Unable to find current ion within file. Using diameter of " .. tostring(diameter) .. "m to calculate cross-sectional area.")
	 else
	    local index = 1
	    local lineValues = {}
	    
	    for value in string.gmatch(massLine,"([^,]+)") do
	    	lineValues[index] = value
		index = index + 1
	    end

	    -- Get the diamater from the line values array in meters
	    diameter = lineValues[2] * 1E-9
	 end
	 
	 -- Calculate total area and save to config file
	 set_file_value(current_ion_diamater_file_name, math.pow(diameter/2,2) * math.pi)

	 return
end

-- Functions to get/set the upstream pressure
local upstream_pressure_file_value = "upstream_pressure_si_units"

function get_upstream_pressure()
	 -- Default is 20 PSI in N/m^2
	 return get_file_value(upstream_pressure_file_value, 137895.14586)
end

function set_upstream_pressure(pressure_psi)
	 -- Convert PSI to N/m^2
	 set_file_value(upstream_pressure_file_value, pressure_psi * 6894.75729)
	 return
end