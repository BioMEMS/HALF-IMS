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

-- Get a value from a generic file
function get_file_value(file_name, default_value)
	 -- Open the temporary file with write permission
	 local file_id = io.open(file_name, "r")

	 -- If the file is not present
	 if (file_id == nil) then
	    -- Write a default value for reading
	    set_file_value(file_name, default_value)
	    
	    -- Open the temporary file with write permission again
	    file_id = io.open(file_name, "r")
	 end
	 
	 -- Set the input to be the file opened
	 io.input(file_id)

	 -- Read the single value from the file
	 local value = io.read()

	 -- Close the file
	 io.close(file_id)
	 
	 return value
end

-- Set a value in a generic file
function set_file_value(file_name, value)
	 -- Open the temporary file with write permission
	 local file_id = io.open(file_name, "w")

	 -- Set file as output
	 io.output(file_id)

	 -- Write desired value
	 io.write(value)
	 
	 -- Close the file
	 io.close(file_id)

	 return
end
