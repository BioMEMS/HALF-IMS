local configuration_file_name = "simulation.config.tmp"
local configuration_separator = ":"

-- Get all functions within a file
function help(file_name)
	 -- Open the temporary file
	 local file_id = io.open(file_name, "r")

	 local found_commands = ""

	 -- If the file was able to be opened
	 if (file_id ~= nil) then
	    -- Variables to track the start and stop position of keywords
	    local start = 0
	    local stop  = 0

	    -- Pull a line from the file to initialize loop
	    local handle = file_id:read("*line")

	    -- While there are values to check
	    while handle do
	    	  -- Attempt to find keyword
		  start, stop = string.find(handle, "function")
		  
	    	  -- If the keyword is right at the start
	    	  if (start == 1) then
	    	     found_commands = found_commands .. handle .. "\n"
		  end
		  
		  handle = file_id:read("*line")
	    end
	 end

	 return found_commands
end

-- Get a numeric value from a generic file
function get_file_value(file_name, default_value)
	 return tonumber(get_raw_file_value(file_name, default_value))
end

-- Get a string value from a generic file
function get_raw_file_value(file_name, default_value)

	 -- Read all configuration file values
	 local config_values = read_all_file_lines(configuration_file_name)
	 
	 -- Find the appropriate index for updating parameter
	 local parameter_index = find_parameter_index(file_name, config_values)

	 -- Set up return value
	 local value = default_value
	 local temp = ""

	 -- If the file is not present
	 if (parameter_index == (#config_values + 1)) then
	    -- Write a default value for reading
	    set_file_value(file_name, default_value)	   
	 else
	    value, temp = string.gsub(config_values[parameter_index], file_name .. configuration_separator, "")
	 end

	 return value
end

-- Set a value in a generic file
function set_file_value(file_name, value)

	 -- Read all configuration file values
	 local config_values = read_all_file_lines(configuration_file_name)

	 -- Find the appropriate index for updating parameter
	 local parameter_index = find_parameter_index(file_name, config_values)
	 
	 -- Open the temporary file with write permission
	 local file_id = io.open(configuration_file_name, "w")

	 -- Update configuration value
	 config_values[parameter_index] = file_name .. configuration_separator .. tostring(value)
	 
	 -- Set file as output
	 io.output(file_id)

	 -- While there are elements in the array to output
	 for i = 1, #config_values do
	       -- Write configuration file value
	       io.write(config_values[i] .. "\n")
	       
	       -- Incremement index
	       i = i + 1
	 end
	 
	 -- Close the file
	 io.close(file_id)

	 return
end

-- Search through the file parameters to find the appropriate index
-- of the given parameter
function find_parameter_index(parameter, parameter_list)
	 -- Declare start and stop variables for parameter searching
	 local start, stop
	 
	 -- For every element in the parameter list
	 for i = 1, #parameter_list do
	     
	     start, stop = string.find(parameter_list[i], parameter)
	     
	     -- If parameter is found, indicated by not nil
	     if (start) then
	     	-- Return current index
	     	return i
	     end
	 end

	 -- If unable to find value, return end of array
	 return (#parameter_list + 1)
end

-- Read all configuration lines and return an array of values
function read_all_file_lines(file_name)
	 -- Initialize a blank array
	 local file_values = {}

	 -- Attempt to open the file
	 local file_id = io.open(file_name, "r")

	 -- If the file could be opened
	 if (file_id ~= nil) then
 	    	 -- Read all contents and place into array	    	     
	    	 for line in file_id:lines() do
		     file_values[#file_values+1] = line
		 end
		 
		 -- Close file descriptor
	 	 file_id:close()
	 end

	 
	 return file_values
end

-- Copy all Ion Library files into current project
function initialize_ion_library()
	 -- Initialize file locations
	 local source_dir = "../ion-library/SIMION Files/"
	 local simion_defs = "m_defs.dat"
	 local source_file = source_dir .. simion_defs
	 local destination_file = simion_defs

	 -- Copy m_defs.dat file to current directory
	 copy_file(source_file, destination_file)

	 return	 
end

-- Copy a file from the source to a destination
function copy_file(src, dest)
	 -- Open all file handles
	 local src_id = io.open(src, "r")
	 local dest_id = io.open(dest, "w")

	 -- Read all contents from source and write to destination
	 dest_id:write(src_id:read("*a"))

	 -- Close all handles
	 dest_id:close()
	 src_id:close()

	 return
end

-- Determine if on the Windows operating system
function windows_operating_system()
	 local temp_path = os.tmpname()

	 local start, stop = string.find(temp_path, ":")
	 return (start < 3) and (stop < 3)
end

-- Append the provided line to the file
function write_to_log(file, line)

	 -- Open provided file
	 local file_id = io.open(file, "a")

	 -- Write provided line
	 file_id:write(line)

	 -- Close file handler
	 io.close(file_id)

	 return
end

-- Functions to set/get an output log file in configuration
local results_output_file_name = "output_log"

-- Get the current output log file name
function get_results_file_name()
	 return get_raw_file_value(results_output_file_name,"output.csv")
end

-- Set the output log file name
function set_results_file_name(name)
	 set_file_value(results_output_file_name, name)
	 return
end
