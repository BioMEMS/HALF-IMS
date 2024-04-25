
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
	 local pair_count = io.read()

	 -- Close the file
	 io.close(file_id)
	 
	 return pair_count
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
