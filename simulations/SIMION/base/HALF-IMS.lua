simion.workbench_program()

-- Update the electrodes functions by importing the file to ensure functionality
simion.import("geometry.lua")
simion.import("electrode_potentials.lua")
simion.import("ions.lua")

-- Adjustable parameters which show up in the GUI under Ion Workbench "Variables" tab

-- Control whether the PE view is frozen after potentials are updated
adjustable freeze_potentials = 1

-- Ion Velocities
local ion_x_velocity = get_ion_x_velocity()
local ion_y_velocity = get_ion_y_velocity()
local ion_z_velocity = get_ion_z_velocity()

-- Ion Acceleration
local ion_x_acceleration = 0

-- Variables to hold file values
local long_electrode_voltage = get_long_electrode_potential()
local short_electrode_voltage = get_short_electrode_potential()
local shutter_electrode_voltage = get_shutter_electrode_potential()
local bias_ring_voltage = get_bias_ring_electrode_potential()
local cur_carrier_gas = get_carrier_gas_rate()
local cur_ion_file = get_current_ion_file()

-- Get the total number of electrode pairs for indexing
local num_electrode_pairs = get_electrode_pair_count()
local num_shutter_electrodes = get_shutter_electrode_count()

-- Initial position for each ion
local initial_x_pos = 0
local initial_y_pos = 0
local initial_z_pos = 0

-- Flag tracking if the particular ion has been scaled
local initial_scaled = 0

-- Variable to hold the log string, log file name, and other logged values
local output_log_line = ""
local output_log_file_name = get_results_file_name()
local output_log_header_line = get_header_line_present()
local deviceXLength = get_device_x_length()*get_grid_x_spacing()
local deviceYLength = get_device_y_length()*get_grid_y_spacing()
local deviceZLength = get_device_z_length()*get_grid_z_spacing()
local deviceZLengthSimulated = get_simulated_z_device_length()

function segment.initialize()
   -- Unclear when this gets called, but it never seems to output a log message...
end

-- Voltage adjustment segment.
function segment.fast_adjust()

  -- Set all electrodes to 0
  for i = 1,(num_shutter_electrodes + 4*num_electrode_pairs + 6) do
    adj_elect[i] = 0
  end
  
  -- Set shutter electrodes to desired voltages
  adj_elect[1] = shutter_electrode_voltage/2
  adj_elect[2] = -shutter_electrode_voltage/2
  
  -- set electodes to the desired voltages
  for i = (num_shutter_electrodes+1),(4*num_electrode_pairs + num_shutter_electrodes + 2),4 do
    adj_elect[i] = -long_electrode_voltage/2 
    adj_elect[i+1] = long_electrode_voltage/2
    adj_elect[i+2] = short_electrode_voltage/2 
    adj_elect[i+3] = -short_electrode_voltage/2    
  end

  -- Set bias ring and detector electrodes to desired voltages
  adj_elect[4*num_electrode_pairs+num_shutter_electrodes+3] = -bias_ring_voltage
  adj_elect[4*num_electrode_pairs+num_shutter_electrodes+4] = bias_ring_voltage
  adj_elect[4*num_electrode_pairs+num_shutter_electrodes+5] = -bias_ring_voltage
  adj_elect[4*num_electrode_pairs+num_shutter_electrodes+6] = bias_ring_voltage
end

-- Adjust acceleration every cycle
function segment.accel_adjust()

end

-- Segment called after each time-step.
function segment.other_actions()

   -- Unclear why initialize is not scaling, so code is placed here with a flag
   -- Keep in mind that this does not seem to act at the first timestep, so there
   -- is a slight delay on when this gets activated which could yield some
   -- inaccuracies
   if (initial_scaled ~= 1) then
     -- Update the pre-defined ion velocity by the scaling parameter
     ion_vx_mm = ion_x_velocity
     ion_vy_mm = ion_y_velocity
     ion_vz_mm = ion_z_velocity

     -- Save the initial position for later logging
     initial_x_pos = ion_px_mm
     initial_y_pos = ion_py_mm
     initial_z_pos = ion_pz_mm

     -- Calculate and save the ion acceleration due to the carrier gas
     ion_x_acceleration = calculate_ion_acceleration(ion_mass)

     -- Set flag to prevent this section from being called again for the current ion
     initial_scaled = 1
   end

   sim_update_pe_surface = 1  -- update display

   -- If the ion has stopped simulating
   if (ion_splat ~= 0) then
     output_log_line = ""

     -- If the output header line is not present
     if (output_log_header_line == 0) then
        output_log_line = output_log_line .. "Electrode Pairs,"     
	output_log_line = output_log_line .. "Device X Length (mm),"
	output_log_line = output_log_line .. "Device Y Length (mm),"
	output_log_line = output_log_line .. "Device Z Length (mm),"
	output_log_line = output_log_line .. "Device Z Length Simulated (mm),"     
	output_log_line = output_log_line .. "Current Ion File,"
	output_log_line = output_log_line .. "Ion Mass (u),"
	output_log_line = output_log_line .. "Ion Charge (e),"
	output_log_line = output_log_line .. "Ion Number,"
	output_log_line = output_log_line .. "Carrier Gas Flow Rate (mL/min),"
	output_log_line = output_log_line .. "Bias Ring (V),"
	output_log_line = output_log_line .. "Shutter Electrode (V),"
	output_log_line = output_log_line .. "Long Electrode (V),"
	output_log_line = output_log_line .. "Short Electrode (V),"
	output_log_line = output_log_line .. "Initial Ion X Position (mm),"
	output_log_line = output_log_line .. "Initial Ion Y Position (mm),"
	output_log_line = output_log_line .. "Initial Ion Z Position (mm),"
	output_log_line = output_log_line .. "Final Ion X Position (mm),"
	output_log_line = output_log_line .. "Final Ion Y Position (mm),"
	output_log_line = output_log_line .. "Final Ion Z Position (mm),"
	output_log_line = output_log_line .. "Detector Pad Hit"
	output_log_line = output_log_line .. "\n"

	-- Write line to CSV
	write_to_log(output_log_file_name, output_log_line)

	-- Save header line state
	output_log_header_line = 1
	set_header_line_present(output_log_header_line)
     end

     -- Log final position, hit metric, and simulation parameters
     output_log_line = ""
     output_log_line = output_log_line .. num_electrode_pairs .. ","     
     output_log_line = output_log_line .. deviceXLength .. ","
     output_log_line = output_log_line .. deviceYLength .. ","
     output_log_line = output_log_line .. deviceZLength .. ","
     output_log_line = output_log_line .. deviceZLengthSimulated .. ","     
     output_log_line = output_log_line .. cur_ion_file .. ","
     output_log_line = output_log_line .. ion_mass .. ","
     output_log_line = output_log_line .. ion_charge .. ","
     output_log_line = output_log_line .. ion_number .. ","
     output_log_line = output_log_line .. cur_carrier_gas .. ","
     output_log_line = output_log_line .. bias_ring_voltage .. ","
     output_log_line = output_log_line .. shutter_electrode_voltage .. ","
     output_log_line = output_log_line .. long_electrode_voltage .. ","
     output_log_line = output_log_line .. short_electrode_voltage .. ","
     output_log_line = output_log_line .. initial_x_pos .. ","
     output_log_line = output_log_line .. initial_y_pos .. ","
     output_log_line = output_log_line .. initial_z_pos .. ","
     output_log_line = output_log_line .. ion_px_mm .. ","
     output_log_line = output_log_line .. ion_py_mm .. ","
     output_log_line = output_log_line .. ion_pz_mm .. ","
     output_log_line = output_log_line .. tostring(hit_detector(ion_px_mm, ion_py_mm))
     output_log_line = output_log_line .. "\n"

     -- Write line to CSV
     write_to_log(output_log_file_name, output_log_line)
     
     -- Update flag as ion is done
     initial_scaled = 0
  end
end

function segment.terminate()
  sim_retain_changed_potentials = freeze_potentials
end

-- Helper functions
function hit_detector(x_pos, y_pos)
  local xScale = get_grid_x_spacing()
  local padStart = get_detector_pad_start() * xScale
  local padStop = get_detector_pad_end() * xScale
  if ((x_pos > padStart) and (x_pos < padStop)) then
      return 1
  else
      return 0
  end    
end