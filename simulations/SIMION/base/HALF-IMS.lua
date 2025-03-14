simion.workbench_program()

-- Update the electrodes functions by importing the file to ensure functionality
simion.import("geometry.lua")
simion.import("electrode_potentials.lua")
simion.import("ions.lua")

-- Import of the SDS collision model
local SDS = simion.import("collision_sds.lua", "noinstall")

-- Adjustable parameters which show up in the GUI under Ion Workbench "Variables" tab

-- Control whether the PE view is frozen after potentials are updated
adjustable freeze_potentials = 1

-- Parameters dictated by configuration file

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
local cur_ion_file = get_current_packet_string()

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
local workbenchGridXSpacing = get_grid_x_spacing()
local workbenchGridYSpacing = get_grid_y_spacing()
local workbenchGridZSpacing = get_grid_z_spacing()
local deviceXLength = get_device_x_length()*workbenchGridXSpacing
local deviceYLength = get_device_y_length()*workbenchGridYSpacing
local deviceZLength = get_device_z_length()*workbenchGridZSpacing
local deviceZLengthSimulated = get_simulated_z_device_length()
local electrodeHeight = get_electrode_height()*workbenchGridYSpacing
local shutterElectrodeLength = get_shutter_electrode_length()*workbenchGridXSpacing
local firstGroundElectrodeLength = get_first_ground_pad_length()*workbenchGridXSpacing
local longElectrodeLength = get_long_electrode_length()*workbenchGridXSpacing
local shortElectrodeLength = get_short_electrode_length()*workbenchGridXSpacing
local secondGroundElectrodeLength = get_second_ground_pad_length()*workbenchGridXSpacing
local biasElectrodeLength = get_bias_ring_length()*workbenchGridXSpacing
local detectorElectrodeLength = get_detector_pad_length()*workbenchGridXSpacing
local workbenchGroupedSetting = get_iob_grouped_setting()
local workbenchGroupedRepulsion = get_iob_grouped_repulsion()
local workbenchRepulsionValue = get_iob_grouped_repulsion_value()
local workbenchTrajectorySetting = get_iob_trajectory_quality()
local chemicalConcentration = get_chemical_concentration()
local upstreamPressure = get_upstream_pressure() / 6894.75729
local carrierPressure = get_carrier_gas_pressure() / 6894.75729

local min_long = get_long_electrode_min_potential()
local max_long = get_long_electrode_max_potential()
local step_long = get_long_electrode_step_potential()

local min_short = get_short_electrode_min_potential()
local max_short = get_short_electrode_max_potential()
local step_short = get_short_electrode_step_potential()

local min_bias = get_bias_electrode_min_potential()
local max_bias = get_bias_electrode_max_potential()
local step_bias = get_bias_electrode_step_potential()

local step_gas = get_carrier_gas_step_flow_rate()
local min_gas = get_carrier_gas_min_flow_rate()
local max_gas = get_carrier_gas_max_flow_rate()

local repetition_count = get_simulate_loop_count()

function segment.flym()
   -- Clear runs to prevent clutter
   sim_trajectory_image_control = get_iob_trajectory_image_control()
   
   -- Preserve original behavior 
   repeat 
	-- For all desired repetitions
      	for repetition=0,repetition_count,1 do 
	    -- For each bias ring voltage
	    for bep=min_bias,max_bias,step_bias do
		-- Set bias ring voltage
		bias_ring_voltage = bep

		-- For each long electrode voltage
		for lep=min_long,max_long,step_long do
		    -- Set long electrode voltage
		    long_electrode_voltage = lep

		    -- For each short electrode voltage
		    for sep=min_short,max_short,step_short do
			-- Set short electrode voltage
			short_electrode_voltage = sep
			
			-- Run simulation
			run()
			
			-- Report progress
			print("Long (V):",lep,"Short (V):",sep,"Bias (V):",bep,"Repeats:",repetition,"/",repetition_count)
		    end
		end
	    end
	 end
   until sim_rerun_flym == 0
end

function segment.initialize()
   -- Call SDS model's initialization function
   SDS.segment.initialize()
end

function segment.initialize_run()
   -- Call SDS model's function
   SDS.segment.initialize_run()
end

function segment.load()
   sim_trajectory_quality = workbenchTrajectorySetting
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
  for i = (num_shutter_electrodes+1),(4*(num_electrode_pairs-1) + num_shutter_electrodes + 2),4 do
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
   -- Call SDS model's acceleration adjustment function
   SDS.segment.accel_adjust()
end

function segment.tstep_adjust()
   -- Call SDS model's function
   SDS.segment.tstep_adjust()
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

   -- Call SDS model other actions after velocity assignment
   SDS.segment.other_actions()

   sim_update_pe_surface = 1  -- update display

   -- If the ion has stopped simulating
   if (ion_splat ~= 0) then
     output_log_line = ""

     -- If the output header line is not present
     if (output_log_header_line == 0) then
     	output_log_line = output_log_line .. "Grouped Flag,"
     	output_log_line = output_log_line .. "Repulsion Setting,"
     	output_log_line = output_log_line .. "Repulsion Value,"
	output_log_line = output_log_line .. "Trajectory Quality,"
        output_log_line = output_log_line .. "Electrode Pairs,"     
	output_log_line = output_log_line .. "Device X Length (mm),"
	output_log_line = output_log_line .. "Device Y Length (mm),"
	output_log_line = output_log_line .. "Device Z Length (mm),"
	output_log_line = output_log_line .. "Device Z Length Simulated (mm),"
	output_log_line = output_log_line .. "Grid X Spacing (mm),"
	output_log_line = output_log_line .. "Grid Y Spacing (mm),"
	output_log_line = output_log_line .. "Grid Z Spacing (mm),"
	output_log_line = output_log_line .. "Electrode Height (mm),"
	output_log_line = output_log_line .. "Shutter Length (mm),"
	output_log_line = output_log_line .. "First Ground Length (mm),"
	output_log_line = output_log_line .. "Long Electrode Length (mm),"
	output_log_line = output_log_line .. "Short Electrode Length (mm),"
	output_log_line = output_log_line .. "Second Ground Length (mm),"
	output_log_line = output_log_line .. "Bias Length (mm),"
	output_log_line = output_log_line .. "Detector Length (mm),"
	output_log_line = output_log_line .. "Current Ion File,"
	output_log_line = output_log_line .. "Upstream Pressure (psi),"
	output_log_line = output_log_line .. "Carrier Pressure (psi),"
	output_log_line = output_log_line .. "Concentration (ppm),"
	output_log_line = output_log_line .. "Ion Mass (u),"
	output_log_line = output_log_line .. "Ion Charge (e),"
	output_log_line = output_log_line .. "Ion Number,"
	output_log_line = output_log_line .. "Carrier Gas Flow Rate (mL/min),"
	output_log_line = output_log_line .. "Bias Ring (V),"
	output_log_line = output_log_line .. "Shutter Electrode (V),"
	output_log_line = output_log_line .. "Long Electrode (V),"
	output_log_line = output_log_line .. "Short Electrode (V),"
	output_log_line = output_log_line .. "Ion X Velocity (mm/usec),"
	output_log_line = output_log_line .. "Ion Y Velocity (mm/usec),"
	output_log_line = output_log_line .. "Ion Z Velocity (mm/usec),"
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
     output_log_line = output_log_line .. workbenchGroupedSetting .. ","
     output_log_line = output_log_line .. workbenchGroupedRepulsion .. ","
     output_log_line = output_log_line .. workbenchRepulsionValue .. ","
     output_log_line = output_log_line .. workbenchTrajectorySetting .. ","	
     output_log_line = output_log_line .. num_electrode_pairs .. ","     
     output_log_line = output_log_line .. deviceXLength .. ","
     output_log_line = output_log_line .. deviceYLength .. ","
     output_log_line = output_log_line .. deviceZLength .. ","
     output_log_line = output_log_line .. deviceZLengthSimulated .. ","
     output_log_line = output_log_line .. workbenchGridXSpacing .. ","
     output_log_line = output_log_line .. workbenchGridYSpacing .. ","
     output_log_line = output_log_line .. workbenchGridZSpacing .. ","
     output_log_line = output_log_line .. electrodeHeight .. ","
     output_log_line = output_log_line .. shutterElectrodeLength .. ","
     output_log_line = output_log_line .. firstGroundElectrodeLength .. ","
     output_log_line = output_log_line .. longElectrodeLength .. ","
     output_log_line = output_log_line .. shortElectrodeLength .. ","
     output_log_line = output_log_line .. secondGroundElectrodeLength .. ","
     output_log_line = output_log_line .. biasElectrodeLength .. ","
     output_log_line = output_log_line .. detectorElectrodeLength .. ","
     output_log_line = output_log_line .. cur_ion_file .. ","
     output_log_line = output_log_line .. upstreamPressure .. ","
     output_log_line = output_log_line .. carrierPressure .. ","
     output_log_line = output_log_line .. chemicalConcentration .. ","
     output_log_line = output_log_line .. ion_mass .. ","
     output_log_line = output_log_line .. ion_charge .. ","
     output_log_line = output_log_line .. ion_number .. ","
     output_log_line = output_log_line .. cur_carrier_gas .. ","
     output_log_line = output_log_line .. bias_ring_voltage .. ","
     output_log_line = output_log_line .. shutter_electrode_voltage .. ","
     output_log_line = output_log_line .. long_electrode_voltage .. ","
     output_log_line = output_log_line .. short_electrode_voltage .. ","
     output_log_line = output_log_line .. ion_x_velocity .. ","
     output_log_line = output_log_line .. ion_y_velocity .. ","
     output_log_line = output_log_line .. ion_z_velocity .. ","
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