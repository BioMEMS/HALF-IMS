-- Import File IO functions
simion.import("file_io.lua")

-- Functions to control the long electrode voltages
local long_electrode_value_file_name = "long_electrode_voltage"
function get_long_electrode_potential()
	 return get_file_value(long_electrode_value_file_name, 0)
end

function set_long_electrode_potential(value)
	 set_file_value(long_electrode_value_file_name, value)
	 return
end

-- Functions to control the short electrode voltages
local short_electrode_value_file_name = "short_electrode_voltage"
function get_short_electrode_potential()
	 return get_file_value(short_electrode_value_file_name, 0)
end

function set_short_electrode_potential(value)
	 set_file_value(short_electrode_value_file_name, value)
	 return
end

-- Functions to control bias ring electrode voltages
local bias_ring_electrode_value_file_name = "bias_ring_electrode_voltage"
function get_bias_ring_electrode_potential()
	 return get_file_value(bias_ring_electrode_value_file_name, 5)
end

function set_bias_ring_electrode_potential(value)
	 set_file_value(bias_ring_electrode_value_file_name, value)
	 return
end

-- Functions to control shutter/gate electrode voltages
local shutter_electrode_value_file_name = "shutter_electrode_voltage"
function get_shutter_electrode_potential()
	 return get_file_value(shutter_electrode_value_file_name, 0)
end

function set_shutter_electrode_potential(value)
	 set_file_value(shutter_electrode_value_file_name, value)
	 return
end

-- Functions to control long electrode potential ranges during simulation
local long_electrode_min_potential_file_name = "long_electrode_min_potential"
local long_electrode_max_potential_file_name = "long_electrode_max_potential"
local long_electrode_step_potential_file_name = "long_electrode_step_potential"

function get_long_electrode_min_potential()
	 return get_file_value(long_electrode_min_potential_file_name, 0)
end

function set_long_electrode_min_potential(value)
	 set_file_value(long_electrode_min_potential_file_name, value)
end

function get_long_electrode_max_potential()
	 return get_file_value(long_electrode_max_potential_file_name, 10)
end

function set_long_electrode_max_potential(value)
	 set_file_value(long_electrode_max_potential_file_name, value)
end

function get_long_electrode_step_potential()
	 return get_file_value(long_electrode_step_potential_file_name, 0.25)
end

function set_long_electrode_step_potential(value)
	 set_file_value(long_electrode_step_potential_file_name, value)
end

-- Functions to control short electrode potential ranges during simulation
local short_electrode_min_potential_file_name = "short_electrode_min_potential"
local short_electrode_max_potential_file_name = "short_electrode_max_potential"
local short_electrode_step_potential_file_name = "short_electrode_step_potential"

function get_short_electrode_min_potential()
	 return get_file_value(short_electrode_min_potential_file_name, 0)
end

function set_short_electrode_min_potential(value)
	 set_file_value(short_electrode_min_potential_file_name, value)
end

function get_short_electrode_max_potential()
	 return get_file_value(short_electrode_max_potential_file_name, 60)
end

function set_short_electrode_max_potential(value)
	 set_file_value(short_electrode_max_potential_file_name, value)
end

function get_short_electrode_step_potential()
	 return get_file_value(short_electrode_step_potential_file_name, 0.25)
end

function set_short_electrode_step_potential(value)
	 set_file_value(short_electrode_step_potential_file_name, value)
end
