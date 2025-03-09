-- Enable GUI libraries
simion.early_access(8.2)

-- Import all helper functions
simion.import("file_io.lua")
simion.import("electrode_potentials.lua")
simion.import("ions.lua")

t = simion.experimental.dialog {
  title='Batch Simulation Parameters',
  {'Log File', get_results_file_name()},
  {'Log File Header Written', get_header_line_present()},	
  {'Long Electrode Min. Potential (V)', get_long_electrode_min_potential()},
  {'Long Electrode Max. Potential (V)', get_long_electrode_max_potential()},
  {'Long Electrode Step Potential (V)', get_long_electrode_step_potential()},
  {'Short Electrode Min. Potential (V)', get_short_electrode_min_potential()},
  {'Short Electrode Max. Potential (V)', get_short_electrode_max_potential()},
  {'Short Electrode Step Potential (V)', get_short_electrode_step_potential()},
  {'Bias Electrode Min. Potential (V)', get_bias_electrode_min_potential()},
  {'Bias Electrode Max. Potential (V)', get_bias_electrode_max_potential()},
  {'Bias Electrode Step Potential (V)', get_bias_electrode_step_potential()},
  {'Ion Packet List', get_ion_files()},
  {'Chemical Concentration (PPM)', get_chemical_concentration()},
  {'Ion Packet X Stop (mm)', get_ion_packet_x_length()},
  {'Carrier Gas Temperature (C)', get_carrier_gas_temperature() - 273.15},
  {'Carrier Gas Pressure (PSI)', get_carrier_gas_pressure() / 6894.76},
  {'Upstream Pressure (PSI)', get_upstream_pressure() / 6894.76},
  {'Carrier Gas Min. Flow Rate (mL/min)', get_carrier_gas_min_flow_rate()},
  {'Carrier Gas Max. Flow Rate (mL/min)', get_carrier_gas_max_flow_rate()},
  {'Carrier Gas Step Flow Rate (mL/min)', get_carrier_gas_step_flow_rate()},
  {'Simulation Grouped Setting', get_iob_grouped_setting()},	
  {'Simulation Repulsion Type', get_iob_grouped_repulsion()},	
  {'Simulation Repulsion Value (C)', get_iob_grouped_repulsion_value()},
  {'Simulation Trajectory Quality', get_iob_trajectory_quality()},	
  {'Simulation Loop Count', get_simulate_loop_count()},
  buttons="&Update|&Close"
}

if(t.result == "Update") then
  set_long_electrode_min_potential(t['Long Electrode Min. Potential (V)'])
  set_long_electrode_max_potential(t['Long Electrode Max. Potential (V)'])
  set_long_electrode_step_potential(t['Long Electrode Step Potential (V)'])
  set_short_electrode_min_potential(t['Short Electrode Min. Potential (V)'])
  set_short_electrode_max_potential(t['Short Electrode Max. Potential (V)'])
  set_short_electrode_step_potential(t['Short Electrode Step Potential (V)'])
  set_bias_electrode_min_potential(t['Bias Electrode Min. Potential (V)'])
  set_bias_electrode_max_potential(t['Bias Electrode Max. Potential (V)'])
  set_bias_electrode_step_potential(t['Bias Electrode Step Potential (V)'])
  set_ion_files(t['Ion Packet List'])
  set_chemical_concentration(t['Chemical Concentration (PPM)'])
  set_carrier_gas_temperature(t['Carrier Gas Temperature (C)'])
  set_carrier_gas_pressure(t['Carrier Gas Pressure (PSI)'])
  set_upstream_pressure(t['Upstream Pressure (PSI)'])
  set_carrier_gas_min_flow_rate(t['Carrier Gas Min. Flow Rate (mL/min)'])
  set_carrier_gas_max_flow_rate(t['Carrier Gas Max. Flow Rate (mL/min)'])
  set_carrier_gas_step_flow_rate(t['Carrier Gas Step Flow Rate (mL/min)'])
  set_iob_grouped_setting(t['Simulation Grouped Setting'])
  set_iob_grouped_repulsion(t['Simulation Repulsion Type'])
  set_iob_grouped_repulsion_value(t['Simulation Repulsion Value (C)'])
  set_iob_trajectory_quality(t['Simulation Trajectory Quality'])
  set_simulate_loop_count(t['Simulation Loop Count'])
  set_ion_packet_x_length(t['Ion Packet X Stop (mm)'])
  set_results_file_name(t['Log File'])
  set_header_line_present(t['Log File Header Written'])
end