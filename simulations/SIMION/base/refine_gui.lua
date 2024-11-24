-- Enable GUI libraries
simion.early_access(8.2)

-- Import all helper functions
simion.import("file_io.lua")
simion.import("geometry.lua")
simion.import("electrode_potentials.lua")
simion.import("ions.lua")

-- Get all grid spacing values for later reuse
gridXSpacing = get_grid_x_spacing()
gridYSpacing = get_grid_y_spacing()
gridZSpacing = get_grid_z_spacing()

t = simion.experimental.dialog {
  title='Model Geometry Parameters',
  {'Drift Region Electrode Pairs', get_electrode_pair_count()},
  {'Electrode Height (mm)', get_electrode_height()*gridYSpacing},
  {'Shutter Electrode Length (mm)', get_shutter_electrode_length()*gridXSpacing},
  {'Shutter Electrode Spacing (mm)', get_shutter_electrode_spacing()*gridXSpacing},
  {'Drift Electrode Spacing (mm)', get_electrode_spacing()*gridXSpacing},
  {'Long Electrode Length (mm)', get_long_electrode_length()*gridXSpacing},
  {'Short Electrode Length (mm)', get_short_electrode_length()*gridXSpacing},
  {'Bias Electrode Length (mm)', get_bias_ring_length()*gridXSpacing},
  {'Bias Electrode Spacing (mm)', get_bias_ring_spacing()*gridXSpacing},
  {'Detector Electrode Length (mm)', get_detector_pad_length()*gridXSpacing},
  {'Detector Electrode Spacing (mm)', get_detector_pad_spacing()*gridXSpacing},
  {'First Ground Electrode Length (mm)', get_first_ground_pad_length()*gridXSpacing},
  {'Second Ground Electrode Length (mm)', get_second_ground_pad_length()*gridXSpacing},
  {'Device Height (mm)', gridYSpacing*get_device_y_length()},
  {'Device Width (mm)', gridZSpacing*get_device_z_length()},
  {'Simulated Device Width (mm)', get_simulated_z_device_length()},
  {'Flow Channel Offset (mm)', get_flow_channel_offset()*gridXSpacing},
  {'Grid X Spacing (mm)', gridXSpacing},
  {'Grid Y Spacing (mm)', gridYSpacing},
  {'Grid Z Spacing (mm)', gridZSpacing},
  buttons="&Update|&Close"
}

if(t.result == "Update") then
   set_electrode_pair_count(t['Drift Region Electrode Pairs'])
   set_electrode_height(t['Electrode Height (mm)'])
   set_shutter_electrode_length(t['Shutter Electrode Length (mm)'])
   set_shutter_electrode_spacing(t['Shutter Electrode Spacing (mm)'])
   set_electrode_spacing(t['Drift Electrode Spacing (mm)'])
   set_long_electrode_length(t['Long Electrode Length (mm)'])
   set_short_electrode_length(t['Short Electrode Length (mm)'])
   set_bias_ring_length(t['Bias Electrode Length (mm)'])
   set_bias_ring_spacing(t['Bias Electrode Spacing (mm)'])
   set_detector_pad_length(t['Detector Electrode Length (mm)'])
   set_detector_pad_spacing(t['Detector Electrode Spacing (mm)'])
   set_first_ground_pad_length(t['First Ground Electrode Length (mm)'])
   set_second_ground_pad_length(t['Second Ground Electrode Length (mm)'])
   set_device_y_length(t['Device Height (mm)'])
   set_device_z_length(t['Device Width (mm)'])
   set_simulated_z_device_length(t['Simulated Device Width (mm)'])
   set_flow_channel_offset(t['Flow Channel Offset (mm)'])
   set_grid_x_spacing(t['Grid X Spacing (mm)'])
   set_grid_y_spacing(t['Grid Y Spacing (mm)'])
   set_grid_z_spacing(t['Grid Z Spacing (mm)'])
end