simion.import("file_io.lua")

-- Local variables for electrode pairs
local default_electrode_file_name = "electrodes_count"
local default_electrode_count = 220

-- Functions to get/set the total number of electrode pairs
function get_electrode_pair_count()
	 -- Get the electrode file value
	 return get_file_value(default_electrode_file_name, default_electrode_count)
end

function set_electrode_pair_count(electrode_count)
	 -- Set the electrode count in the default file name
	 set_file_value(default_electrode_file_name, electrode_count)
	 return
end

-- Function to get the minimum grid spacing
function get_grid_min_spacing()
	 -- Get known x and y grid spacing
	 local temp = tonumber(get_grid_y_spacing())
	 local spacing = tonumber(get_grid_x_spacing())

	 -- Determine if y spacing is smaller than x spacing
	 if(temp < spacing) then
	 	 -- Update return value if smaller
	 	 spacing = temp
	 end	 

	 -- Get known z grid spacing
	 temp = tonumber(get_grid_z_spacing())

	 -- Determine if z spacing is smaller than x/y spacing
	 if(temp < spacing) then
	 	 -- Update return value if smaller
	 	 spacing = temp
	 end
	 
	 return spacing
end

-- Local variables for grid x spacing in millimeters
local default_grid_x_spacing_file_name = "grid_x_spacing"
local default_grid_x_spacing = 5E-3

-- Functions to get/set grid spacing
function get_grid_x_spacing()
	 -- Get the grid spacing value from file
	 return get_file_value(default_grid_x_spacing_file_name, default_grid_x_spacing)
end

function set_grid_x_spacing(spacing)
	 -- Set the grid spacing file value
	 set_file_value(default_grid_x_spacing_file_name, spacing)
	 return
end

-- Local variables for grid y spacing in millimeters
local default_grid_y_spacing_file_name = "grid_y_spacing"
local default_grid_y_spacing = 5E-3

-- Functions to get/set grid spacing
function get_grid_y_spacing()
	 -- Get the grid spacing value from file
	 return get_file_value(default_grid_y_spacing_file_name, default_grid_y_spacing)
end

function set_grid_y_spacing(spacing)
	 -- Set the grid spacing file value
	 set_file_value(default_grid_y_spacing_file_name, spacing)
	 return
end

-- Local variables for grid z spacing in millimeters
local default_grid_z_spacing_file_name = "grid_z_spacing"
local default_grid_z_spacing = 1

-- Functions to get/set grid spacing
function get_grid_z_spacing()
	 -- Get the grid spacing value from file
	 return get_file_value(default_grid_z_spacing_file_name, default_grid_z_spacing)
end

function set_grid_z_spacing(spacing)
	 -- Set the grid spacing file value
	 set_file_value(default_grid_z_spacing_file_name, spacing)
	 return
end

-- Local variables for electrode height
local default_electrode_height_file_name = "electrode_height"
local default_electrode_height = 20E-6

-- Functions to get/set universal electrode height
function get_electrode_height()
	 -- Get electrode height value in grid units
	 return get_file_value(default_electrode_height_file_name, default_electrode_height)/get_grid_y_spacing()
end

function set_electrode_height(height)
	 -- Set electrode height value in grid units
	 set_file_value(default_electrode_height_file_name, height)
	 return
end

-- Local variables for flow channel offset
local default_flow_channel_file_name = "flow_channel_offset"
local default_flow_channel_offset = 10E-3

-- Functions to get/set flow channel offset
function get_flow_channel_offset()
	 return get_file_value(default_flow_channel_file_name, default_flow_channel_offset)/get_grid_x_spacing()
end

function set_flow_channel_offset(offset)
	 set_file_value(default_flow_channel_file_name, offset)
	 return
end

-- Local variables for shutter electrode count
local default_shutter_electrode_count_file_name = "shutter_electrode_count"
local default_shutter_electrode_count = 2

-- Functions to get/set shutter electrode count
function get_shutter_electrode_count()
	 return get_file_value(default_shutter_electrode_count_file_name, default_shutter_electrode_count)
end

function set_shutter_electrode_count(count)
	 set_file_value(default_shutter_electrode_count_file_name, count)
	 return
end

-- Local variables for shutter electrode length
local default_shutter_electrode_length_file_name = "shutter_electrode_length"
local default_shutter_electrode_length = 10E-2

-- Functions to get/set shutter electrode length
function get_shutter_electrode_length()
	 return get_file_value(default_shutter_electrode_length_file_name, default_shutter_electrode_length)/get_grid_x_spacing()
end

function set_shutter_electrode_length(length)
	 set_file_value(default_shutter_electrode_length_file_name, length)
	 return
end

-- Local variables for shutter electrode spacing
local default_shutter_electrode_spacing_file_name = "shutter_electrode_spacing"
local default_shutter_electrode_spacing = 100E-3

-- Functions to get/set shutter electrode spacing
function get_shutter_electrode_spacing()
	 return get_file_value(default_shutter_electrode_spacing_file_name, default_shutter_electrode_spacing)/get_grid_x_spacing()
end

function set_shutter_electrode_spacing(spacing)
	 set_file_value(default_shutter_electrode_spacing_file_name, spacing)
	 return
end

-- Local variables for long & short electrode spacing
local default_electrode_spacing_file_name = "drift_electrode_spacing"
local default_electrode_spacing = 10E-3

-- Functions to get/set long & short electrode spacing
function get_electrode_spacing()
	 return get_file_value(default_electrode_spacing_file_name, default_electrode_spacing)/get_grid_x_spacing()
end

function set_electrode_spacing(spacing)
	 set_file_value(default_electrode_spacing_file_name, spacing)
	 return
end

-- Local variables for short electrode length
local default_short_electrode_length_file_name = "short_electrode_length"
local default_short_electrode_length = 10E-3

-- Functions to get/set short electrode length
function get_short_electrode_length()
	 return get_file_value(default_short_electrode_length_file_name, default_short_electrode_length)/get_grid_x_spacing()
end

function set_short_electrode_length(length)
	 set_file_value(default_short_electrode_length_file_name, length)
	 return
end

-- Local variables for long electrode length
local default_long_electrode_length_file_name = "long_electrode_length"
local default_long_electrode_length = 50E-3

-- Functions to get/set long electrode length
function get_long_electrode_length()
	 return get_file_value(default_long_electrode_length_file_name, default_long_electrode_length)/get_grid_x_spacing()
end

function set_long_electrode_length(length)
	 set_file_value(default_long_electrode_length_file_name, length)
	 return
end

-- Local variables for bias ring spacing
local default_bias_ring_spacing_file_name = "bias_ring_spacing"
local default_bias_ring_spacing = 20E-3

-- Functions to get/set bias ring spacing
function get_bias_ring_spacing()
	 return get_file_value(default_bias_ring_spacing_file_name, default_bias_ring_spacing)/get_grid_x_spacing()
end

function set_bias_ring_spacing(spacing)
	 set_file_value(default_bias_ring_spacing_file_name, spacing)
	 return
end

-- Local variables for bias ring length
local default_bias_ring_length_file_name = "bias_ring_length"
local default_bias_ring_length = 10E-3

-- Functions to get/set bias ring length
function get_bias_ring_length()
	 return get_file_value(default_bias_ring_length_file_name, default_bias_ring_length)/get_grid_x_spacing()
end

function set_bias_ring_length(length)
	 set_file_value(default_bias_ring_length_file_name,length)
	 return
end

-- Local variables for detector pad spacing
local default_detector_pad_spacing_file_name = "detector_pad_spacing"
local default_detector_pad_spacing = 100E-3

-- Functions to get/set detector pad spacing
function get_detector_pad_spacing()
	 return get_file_value(default_detector_pad_spacing_file_name, default_detector_pad_spacing)/get_grid_x_spacing()
end

function set_detector_pad_spacing(spacing)
	 set_file_value(default_detector_pad_spacing_file_name, spacing)
	 return
end

-- Local variables for detector pad length
local default_detector_pad_length_file_name = "detector_pad_length"
local default_detector_pad_length = 1

-- Functions to get/set detector pad length
function get_detector_pad_length()
	 return get_file_value(default_detector_pad_length_file_name, default_detector_pad_length)/get_grid_x_spacing()
end

function set_detector_pad_length(length)
	 set_file_value(default_detector_pad_length_file_name, length)
	 return
end

-- Function to get the position of the detector pads
function get_detector_pad_start()
	 return (get_device_x_length() - get_flow_channel_offset() - get_detector_pad_length())
end

function get_detector_pad_end()
	 return (get_detector_pad_start() + get_detector_pad_length())
end

-- Functions to get the number of grid units in the X-direction
function get_shutter_pattern_length()
	 return get_shutter_electrode_count()*(get_shutter_electrode_length() + get_shutter_electrode_spacing())/2 + get_flow_channel_offset()
end

function get_electrode_pattern_length()
	 return get_electrode_pair_count()*(2*get_electrode_spacing() + get_short_electrode_length() + get_long_electrode_length())
end

function get_detector_pattern_length()
	 return get_bias_ring_length() + get_bias_ring_spacing() + get_detector_pad_length() + get_detector_pad_spacing() + get_flow_channel_offset()
end

function get_device_x_length()

	 -- Calculate the shutter electrode pattern in grid units
	 local shutter_electrode_pattern = get_shutter_pattern_length()

	 -- Calculate the channel electrode pattern in grid units
	 local electrode_pattern = get_electrode_pattern_length()

	 -- Calculate detector pattern in grid units
	 local detector_pattern = get_detector_pattern_length()

	 -- Calculate pattern to determine
	 local pattern = shutter_electrode_pattern + electrode_pattern + detector_pattern

	 -- Return sum of all grid units for total x length
	 return check_exceeded_grid_units("x", pattern)
end

-- Local variables for Y-direction dimension
local default_y_direction_file_name = "y_direction_length"
local default_y_direction = 50E-3

-- Functions to get/set the number of grid units in the Y-direction
function get_device_y_length()
	 local length = get_file_value(default_y_direction_file_name, default_y_direction)/get_grid_y_spacing()

	 return check_exceeded_grid_units("y", length)
end

function set_device_y_length(length)
	 set_file_value(default_y_direction_file_name, length)
	 return
end

-- Local variables for Z-direction dimension
local default_z_direction_file_name = "z_direction_length"
local default_z_direction = 1

-- Functions to get/set the number of grid units in the Z-direction
function get_device_z_length()
	 local length = get_file_value(default_z_direction_file_name, default_z_direction)/get_grid_z_spacing()

	 return check_exceeded_grid_units("z", length)
end

function set_device_z_length(length)
	 set_file_value(default_z_direction_file_name, length)
	 return
end

-- Define generic functions which can be used by specific getters/setters

-- Check if grid units exceed some known limits
function check_exceeded_grid_units(axis, grid_units)
	 local value = grid_units

	 
	 -- Determine if grid units calculated are outside acceptable parameters
	 if ((axis == "z") and (grid_units < 0)) then
	    print("Increasing to 3 grid units for ", axis, " axis.")
	    value = 0
	 end

	 if ((axis ~= "z") and (grid_units < 3)) then
	    print("Increasing to 3 grid units for ", axis, " axis.")
	    value = 3
	 end

	 if (grid_units > (math.pow(2,30)-1)) then
	    print("Limiting to 2^30-1 grid units for ", axis, " axis.")
	    value = math.pow(2,30)-1
	 end

	 return value
end
