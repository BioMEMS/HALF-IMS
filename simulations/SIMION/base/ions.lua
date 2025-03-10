simion.import("file_io.lua")
simion.import("geometry.lua")

--Function to build an ion file and place in particles directory
function build_ion_file(name, masses, charges)

	 local fileID = io.open(name,"w")
	 local particles = 0
	 local ions = 0
	 
	 --Write opening FLY2 line
	 fileID:write("particles {\n  coordinates = 1")
	 
	 contentFormatString = ",\n  standard_beam {\n    n = %s,\n    tob = 0,\n    mass = %s,\n    charge = %s,\n    cwf = 1,\n    color = %s,\n    position = line_distribution {\n      first = vector(0, 0, 0),\n      last = vector(0, 0, 0)\n    },\n    velocity = vector(1, 0, 0)\n  }"
	 
	 --For each particle mass
	 for i=1,#masses do
	     -- If charge is neutral
	     if(charges[i] == 0) then
	        -- Assume carrier gas
	        particles = calculate_atom_count()
	     else
		-- Calculate ion count per concentration
	        particles = calculate_ion_count()
		ions = ions + particles
	     end

	     --Write particle beam to file
	     fileID:write(string.format(contentFormatString, tostring(particles), tostring(masses[i]), tostring(charges[i]), tostring(i)))

	     -- If charges are not neutral
	     if(charges[i] ~= 0) then
	        -- Calculate non-ionized particles
	     	particles = calculate_atom_count() - particles

		--fileID:write(string.format(contentFormatString, tostring(particles), tostring(masses[i]), "0", "0"))
	     end
	 end 

	 -- If using "Coulomb" repulsion type
	 if(get_iob_grouped_repulsion() == "coulomb") then
	     -- Update repulsion charge to be charge particle count times elementary charge
	     set_iob_grouped_repulsion_value(ions * 1E-19)
	 end
	 
	 --Write closing FLY2 line
	 fileID:write("\n}\n")

	 --Close file handler
	 fileID:close();
	 
	 return
end

--Functions to set/get a list of ion files to process
local simulation_ions_file_name = "simulate_files_list"
function get_ion_files()
	 return get_raw_file_value(simulation_ions_file_name, "")
end

function set_ion_files(value_list)
	 -- Write generated string to the file
	 set_file_value(simulation_ions_file_name, value_list)
	 return
end

--Functions to set/get the current ion file
local simulation_current_ion_file_name = "simulate_current_ion_file"
local simulation_current_ion_packet_string_file_name = "simulate_current_packet_string"

function get_current_ion_file()
	 return get_raw_file_value(simulation_current_ion_file_name, "NONE")
end

function get_current_packet_string()
	 return get_raw_file_value(simulation_current_ion_packet_string_file_name, "NONE")
end

function set_current_ion_file(packet_string)
	 local temp = "current_packet.fly2"
	 local tempFileName = "temp.fly2"

	 -- Parse packet string
 	 local masses = {}
	 local charges = {}
	 for pair in string.gmatch(packet_string, "([^|]+)") do
	     local table = {}
	     local count = 1
	     for value in string.gmatch(pair, "([^:]+)") do
	     	 table[count] = value
		 count = count + 1
	     end
	     masses[#masses + 1] = table[1]
	     charges[#charges + 1] = table[2]
	 end

	 -- Dynamically build ion file
	 build_ion_file(temp, masses, charges)	 

	 -- Copy file to local directory
	 copy_file(temp, tempFileName)

	 -- Modify file to account for current geometry
	 local electrodeHeight = 2*math.ceil(get_electrode_height())
	 local yDimension = get_device_y_length()
	 local zDimension = get_device_z_length()

	 -- Open file with read/write
	 local tempID = io.open(tempFileName,"r")
	 local fileID = io.open(temp,"w")
	 
	 -- Get ion count
	 local ionPacketEnd = tostring(get_ion_packet_x_length())
	 
	 -- For every line in the file
	 for line in tempID:lines() do
	     -- If it matches the first position vector
	     if (string.match(tostring(line), " *first =.*")) then
	     	 -- Updating Y position
	     	 fileID:write("      first = vector(0, " .. tostring(electrodeHeight) .. ", " .. tostring(0.2*zDimension) .. "),")
	     elseif (string.match(tostring(line), " *last =.*")) then
	     	 -- Updating Y position end
     	     	 fileID:write("      last = vector(" .. ionPacketEnd .. ", " .. tostring(yDimension - electrodeHeight) .. " , " .. tostring(0.8*zDimension) .. ")")
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

	 -- Set file name in configuration and packet string
	 set_file_value(simulation_current_ion_file_name, temp)
	 set_file_value(simulation_current_ion_packet_string_file_name, packet_string)
	 return
end

--Function to calculate the total number of ions based upon concentration
function calculate_ion_count()

	 -- Reduce total atom count by ionization percentage
	 local ions = get_ionization_percentage() * calculate_atom_count()
	 
	 return math.floor(ions)
end

function calculate_atom_count()
	 local chemicalConcentration = get_chemical_concentration()
	 local deviceVolume = (get_ion_packet_x_length()*get_grid_x_spacing())*(get_device_y_length()*get_grid_y_spacing())*(get_device_z_length()*get_grid_z_spacing())	 

	 -- Determine 
	 local chemicalMoles = (chemicalConcentration * deviceVolume * get_carrier_gas_density()) / (get_carrier_gas_molar_mass() * 1E6)

	 return math.floor(chemicalMoles * 6.02214076E23)
end

-- Functions to set/get the ion packet X-dimension
local ion_packet_x_length_file_value="ion_packet_x_length"
function get_ion_packet_x_length()
	 return get_file_value(ion_packet_x_length_file_value, 10E-6)
end

function set_ion_packet_x_length(length)
	 set_file_value(ion_packet_x_length_file_value, length)
	 return
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
local carrier_gas_pressure_file_name = "carrier_gas_pressure"

function get_carrier_gas_pressure()
	 return get_file_value(carrier_gas_pressure_file_name, 137895)
end

function set_carrier_gas_pressure(value)
	 -- Accept PSI value and convert to N/m^2
	 set_file_value(carrier_gas_pressure_file_name, value * 6894.76)
end

--Functions to get/set the carrier gas temperature
local carrier_gas_temperature_file_name = "carrier_gas_temperature_kelvin"

function get_carrier_gas_temperature()
	 return get_file_value(carrier_gas_temperature_file_name, 298.15)
end

function set_carrier_gas_temperature(value)
	 -- Accept Celsius and convert to Kelvin
	 set_file_value(carrier_gas_temperature_file_name, value + 273.15)
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

-- Functions to get/set ion flying parameters
local iob_grouped_setting_file_value = "iob_grouped_setting"
local iob_grouped_repulsion_file_value = "iob_grouped_repulsion"
local iob_grouped_repulsion_value_file_value = "iob_grouped_repulsion_value"

function get_iob_grouped_setting()
	 return get_file_value(iob_grouped_setting_file_value, 0)
end

function set_iob_grouped_setting(flag)
	 set_file_value(iob_grouped_setting_file_value, flag)
	 return
end

function get_iob_grouped_repulsion()
	 return get_raw_file_value(iob_grouped_repulsion_file_value, "none")
end

function set_iob_grouped_repulsion(setting)
	 set_file_value(iob_grouped_repulsion_file_value, setting)
	 return
end

function get_iob_grouped_repulsion_value()
	 return get_file_value(iob_grouped_repulsion_value_file_value, 1.6E-19)
end

function set_iob_grouped_repulsion_value(setting)
	 set_file_value(iob_grouped_repulsion_value_file_value, setting)
	 return
end

-- Functions to get/set the ion trajectory quality
local iob_trajectory_quality_file_value = "iob_trajectory_quality_setting"

function get_iob_trajectory_quality()
	 return get_file_value(iob_trajectory_quality_file_value, 0)
end

function set_iob_trajectory_quality(value)
	 set_file_value(iob_trajectory_quality_file_value, value)
	 return
end

-- Functions to assist in the generation of ion counts
local chemical_properties_database_file_value = "chemical_properties_database_file"
local ionization_percentage_file_vale = "ionization_percentage"
local carrier_gas_density_file_value = "carrier_gas_density"
local carrier_gas_molar_mass_file_value = "carrier_gas_molar_mass"

function set_carrier_gas(name)
	 if (name == "nitrogen") then
	    set_carrier_gas_density(0.00125)
	    set_carrier_gas_molar_mass(28.02)
	 elseif (name == "oxygen") then
	    set_carrier_gas_density(0.001429)
	    set_carrier_gas_molar_mass(31.999)
	 elseif (name == "helium") then
	    set_carrier_gas_density(0.0001786)
	    set_carrier_gas_molar_mass(4.0026)
	 end

	 return
end

function set_ionization_type(name)

	 if (name == "photoionization") then
	    set_ionization_percentage(0.3)
	 elseif (name == "plasma") then
	    set_ionization_percentage(0.9)
	 elseif (name == "radioactive") then
	    set_ionization_percentage(0.9)
	 end

	 return
end

function get_carrier_gas_density()
	 return get_file_value(carrier_gas_density_file_value, 0.00125)
end

function set_carrier_gas_density(value)
	 set_file_value(carrier_gas_density_file_value, value)
	 return
end

function get_carrier_gas_molar_mass()
	 return get_file_value(carrier_gas_molar_mass_file_value, 28.02)
end

function set_carrier_gas_molar_mass(value)
	 set_file_value(carrier_gas_molar_mass_file_value, value)
	 return
end

function get_ionization_percentage()
	 return get_file_value(ionization_percentage_file_vale, 0.3)
end

function set_ionization_percentage(value)
	 set_file_value(ionization_percentage_file_vale, value)
	 return
end

function calculate_chemical_density(mass)
	 local pressure = get_carrier_gas_pressure()
	 local temperature = 298
	 return (mass * pressure) / (8.3145 * temperature)
end

function get_chemical_properties_database_file()
	 return get_raw_file_value(chemical_properties_database_file_value, "properties.tmp")
end

function set_chemical_properties_database_file(path)
	 set_file_value(chemical_properties_database_file_value, path)
	 return
end

function get_chemical_properties(mass)
	 -- Get all CSV lines in the database

	 -- Read all configuration file values
	 local config_values = read_all_file_lines(get_chemical_properties_database_file())
	 
	 -- Find the appropriate index for updating parameter
	 local parameter_index = find_parameter_index(mass, config_values)

	 -- Set up return value
	 local value = default_value
	 local temp = ""
	 
	 -- Remove key from string and convert to numeric
	 value, temp = string.gsub(config_values[parameter_index], mass .. ":", "")

	 molarMass = tonumber(value)
	 
	 -- Calculate density from molar mass
	 chemicalDensity = calculate_chemical_density(molarMass)

	 -- Return table with both values
	 return {["mass"] = molarMass, ["density"] = chemicalDensity}
end
