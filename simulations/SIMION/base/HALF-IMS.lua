simion.workbench_program()

-- Update the electrodes functions by importing the file to ensure functionality
simion.import("geometry.lua")
simion.import("electrode_potentials.lua")

-- Adjustable parameters which show up in the GUI under Ion Workbench "Variables" tab

-- Control whether the PE view is frozen after potentials are updated
adjustable freeze_potentials = 1

-- Velocity scale factors
adjustable ion_x_velocity_scale = 1
adjustable ion_y_velocity_scale = 1
adjustable ion_z_velocity_scale = 1

-- Variables to hold file values
local long_electrode_voltage = get_long_electrode_potential()
local short_electrode_voltage = get_short_electrode_potential()
local shutter_electrode_voltage = get_shutter_electrode_potential()
local bias_ring_voltage = get_bias_ring_electrode_potential()

-- Get the total number of electrode pairs for indexing
local num_electrode_pairs = get_electrode_pair_count()
local num_shutter_electrodes = get_shutter_electrode_count()

-- Initial position for each ion
local initial_x_pos = 0
local initial_y_pos = 0
local initial_z_pos = 0

-- Flag tracking if the particular ion has been scaled
local initial_scaled = 0

function segment.initialize()
   -- Unclear when this gets called, but it never seems to output a log message...
end

-- Voltage adjustment segment.
function segment.fast_adjust()

  -- Set all electrodes to 0
  for i = 1,num_electrode_pairs do
    adj_elect[i] = 0
  end
  
  -- Set shutter electrodes to desired voltages
  adj_elect[1] = shutter_electrode_voltage/2
  adj_elect[2] = -shutter_electrode_voltage/2

  -- set electodes to the desired voltages
  for i = (num_shutter_electrodes+1),(4*num_electrode_pairs),4 do
    adj_elect[i] = -short_electrode_voltage/2 
    adj_elect[i+1] = short_electrode_voltage/2
    adj_elect[i+2] = long_electrode_voltage/2 
    adj_elect[i+3] = -long_electrode_voltage/2    
  end

  -- Set bias ring electrodes to desired voltages
  adj_elect[4*num_electrode_pairs+3] = bias_ring_voltage
  adj_elect[4*num_electrode_pairs+4] = -bias_ring_voltage
  
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
     ion_vx_mm = ion_vx_mm*ion_x_velocity_scale
     ion_vy_mm = ion_vy_mm*ion_y_velocity_scale
     ion_vz_mm = ion_vz_mm*ion_z_velocity_scale

     -- Save the initial position for later logging
     initial_x_pos = ion_px_mm
     initial_y_pos = ion_py_mm
     initial_z_pos = ion_pz_mm
     
     -- Set flag to prevent this section from being called again for the current ion
     initial_scaled = 1
   end

   sim_update_pe_surface = 1  -- update display

   if (ion_splat ~= 0) then
     -- Log final position and hit metric
     print(ion_number, initial_x_pos, ",", initial_y_pos, ",", initial_z_pos, ",", ion_px_mm, ",", ion_py_mm, ",", ion_pz_mm, ",", hit_detector(ion_px_mm, ion_py_mm))
     
     -- Update flag as ion is done
     initial_scaled = 0
  end
end

function segment.terminate()
  sim_retain_changed_potentials = freeze_potentials
end

-- Helper functions
function hit_detector(x_pos, y_pos)
  return (x_pos > get_detector_pad_start()) and (x_pos < get_detector_pad_end())
end