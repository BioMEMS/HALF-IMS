simion.exe lua refine_gui.lua
simion.exe --noprompt --nogui gem2pa HALF-IMS.gem HALF-IMS.pa#
simion.exe --noprompt --nogui refine HALF-IMS.pa#
del HALF-IMS.iob
@echo off
echo Before running the 'simulate.lua' program, please load the generated PA, open a workbench with the refined PA and save as 'HALF-IMS.iob' to comply with the SIMION workflow.
set /p res="Press enter to complete."
