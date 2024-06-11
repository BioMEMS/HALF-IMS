-- Import File IO functions
simion.import("file_io.lua")

-- Functions to control the long electrode voltages
local long_electrode_value_file_name = "long_electrode_voltage.tmp"
function get_long_electrode_potential()
	 return tonumber(get_file_value(long_electrode_value_file_name, 0))
end

function set_long_electrode_potential(value)
	 set_file_value(long_electrode_value_file_name, value)
end

-- Functions to control the short electrode voltages
local short_electrode_value_file_name = "short_electrode_voltage.tmp"
function get_short_electrode_potential()
	 return tonumber(get_file_value(short_electrode_value_file_name, 0))
end

function set_short_electrode_potential(value)
	 set_file_value(short_electrode_value_file_name, value)
end

-- Functions to control bias ring electrode voltages
local bias_ring_electrode_value_file_name = "bias_ring_electrode_voltage.tmp"
function get_bias_ring_electrode_potential()
	 return tonumber(get_file_value(bias_ring_electrode_value_file_name, 5))
end

function set_bias_ring_electrode_potential(value)
	 set_file_value(bias_ring_electrode_value_file_name, value)
end

-- Functions to control shutter/gate electrode voltages
local shutter_electrode_value_file_name = "shutter_electrode_voltage.tmp"
function get_shutter_electrode_potential()
	 return tonumber(get_file_value(shutter_electrode_value_file_name, 0))
end

function set_shutter_electrode_potential(value)
	 set_file_value(shutter_electrode_value_file_name, value)
end

