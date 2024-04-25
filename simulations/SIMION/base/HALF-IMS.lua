simion.workbench_program()

-- Update the electrodes functions by importing the file to ensure functionality
simion.import("geometry.lua")

-- Adjustable parameters which show up in the GUI under Ion Workbench "Variables" tab
adjustable long_electrode_voltage = 0
adjustable short_electrode_voltage = 0
adjustable shutter_electrode_voltage = 0
adjustable bias_ring_voltage = 5
adjustable freeze_potentials = 1
adjustable ion_velocity_scale = 1
-- Get the total number of electrode pairs for indexing
local num_electrode_pairs = get_electrode_pair_count()


-- Offset for the indexing
local num_shutter_electrodes = 2

-- Flag to track if velocity has been scaled on ions
local velocity_scaled = 0

function segment.initialize()
   -- Update the pre-defined ion velocity by the scaling parameter (does not seem to be working?)
   ion_vx_mm = ion_vx_mm*ion_velocity_scale
   ion_vy_mm = ion_vy_mm*ion_velocity_scale
   ion_vz_mm = ion_vz_mm*ion_velocity_scale

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
  print("test",ion_vx_mm)
end

-- Segment called after each time-step.
function segment.other_actions()

   -- Update the pre-defined ion velocity by the scaling parameter (does not seem to be working?)
   ion_vx_mm = ion_vx_mm*ion_velocity_scale
   ion_vy_mm = ion_vy_mm*ion_velocity_scale
   ion_vz_mm = ion_vz_mm*ion_velocity_scale

   print(ion_number, ion_vx_mm)
   
  sim_update_pe_surface = 1  -- update display

  if (ion_splat ~= 0) then
     print(ion_number, ",", ion_px_mm, ",", ion_py_mm, ",", ion_pz_mm, ",", hit_detector(ion_px_mm, ion_py_mm))
  end
end

function segment.terminate()
  sim_retain_changed_potentials = freeze_potentials

  --Reset flag
  velocity_scaled = 0
end

-- Helper functions
function hit_detector(x_pos, y_pos)
  return (x_pos > get_detector_pad_start()) and (x_pos < get_detector_pad_end())
end