simion.workbench_program()
adjustable long_electrode_voltage = 0
adjustable short_electrode_voltage = 0
adjustable shutter_electrode_voltage = 0
adjustable bias_ring_voltage = 5
adjustable num_electrode_pairs = 8
adjustable freeze_potentials = 1
adjustable ion_velocity_scale = 1

-- Offset for the indexing
local num_shutter_electrodes = 2

-- Flag to track if velocity has been scaled on ions
local velocity_scaled = 0

function segment.initialize()

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


function segment.accel_adjust()
   -- Divide the scale value from acceleration to adjust
   ion_ax_mm = ion_ax_mm/ion_velocity_scale
   ion_ay_mm = ion_ay_mm/ion_velocity_scale
   ion_az_mm = ion_az_mm/ion_velocity_scale
end

-- Segment called after each time-step.
function segment.other_actions()

  sim_update_pe_surface = 1  -- update display
    
end

function segment.terminate()
  sim_retain_changed_potentials = freeze_potentials

  --Reset flag
  velocity_scaled = 0
  print("called! ", velocity_scaled)
end
