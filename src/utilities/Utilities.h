#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <iostream>
#include <vector>

#define ELEMENTARY_CHARGE 1.6E-19
#define AMU_TO_GRAMS 1.660539E-24
#define DETECTOR_BOARD_IDEAL_RESISTANCE 300E9
#define EFIELD_TO_TOWNSEND 1E21
#define PSI_TO_PASCALS 6894.757
#define BOLTZMANN_CONSTANT 1.38E-23

namespace Utilities{
  //Simple limits structure to hold values
  struct Limits{
    unsigned Rows;
    unsigned Columns;
    double MinimumValue;
    double MaximumValue;
  };

  struct ConvertedData{
    double value;
    bool error;
    std::string msg;
  };
  
  //Name:    ConvertValue_Double
  //Purpose: Convert a provided string to a double without exception.
  //Input:   item (std::string) - The string value to convert.
  //Output:  value (ConvertedData) - A struct with value, error flag, and error message.
  //Note:    Error message only valid if error flag is true.
  ConvertedData ConvertValue_Double(std::string item);

  //Name:    ConvertValue_MassGrams
  //Purpose: Convert atomic mass units (AMU) to grams
  //Input:   mass (double) - The mass, in AMU, to convert.
  //Output:  mass (double) - The mass, in grams.
  //Note:    None
  ConvertedData ConvertValue_MassUnits(double mass);

  //Name:    ConvertValue_MassGrams
  //Purpose: Convert grams to atomic mass units (AMU)
  //Input:   mass (double) - The mass, in grams, to convert.
  //Output:  mass (double) - The mass, in AMU.
  //Note:    None
  ConvertedData ConvertValue_MassGrams(double mass);
  
  //Name:    ConvertValue_MassGrams
  //Purpose: Convert grams to atomic mass units (AMU)
  //Input:   temperature (double) - The temperature of the carrier gas, in Celsius, between the electrodes.
  //         pressure (double) - The pressure of the carrier gas, in PSI, between the electrodes.
  //         gap (double) - The gap size, in meters, between the electrodes.
  //         voltage (double) - The potential, in Volts, between the electrodes.
  //Output:  setting (double) - The Townsends value calculated for the provided parameters.
  //Note:    Function assumes a parallel plate capacitor configuration of the electrodes.
  double ConvertValue_Townsends(double temperature, double pressure, double gap, double voltage);

  //Name:    CalculateAtoms
  //Purpose: Convert current to number of atoms per Faraday Cup equations
  //Input:   current (double) - The current, in Amps, over the timeframe.
  //         time (double) - The amount of time, in seconds, to use for calculation.
  //Output:  atoms (double) - The number of atoms calculated.
  //Note:    Fractional atoms will be returned depending on inputs.
  double CalculateAtoms(double current, double time);

  //Name:    CalculateCurrent
  //Purpose: Convert number of atoms to current per Faraday Cup equations.
  //Input:   atoms (double) - The number of atoms.
  //         time (double) - The amount of time, in seconds, to use for calculations.
  //Output:  current (double) - The current calculated to be output for the number of atoms per unit time.
  //Note:    None
  double CalculateCurrent(double atoms, double time);

  //Name:    CalculateCurrent
  //Purpose: Convert the detector voltage value to an ideal current per Detector Board equations.
  //Input:   voltage (double) - The voltage change, in Volts, corresponding to the signal.
  //Output:  current (double) - The calculated current.
  //Note:    None
  double CalculateCurrent(double voltage);

  //Name:    CalculateResponse
  //Purpose: Convert supplied signal and background value into a response value
  //Input:   signal (double) - The signal value.
  //         background (double) - The background signal value.
  //Output:  response (double) - A percentage value indicating the relative response.
  //Note:    Signal and background units must match for meaningful calculation.
  double CalculateResponse(double signal, double background);

  //Name:    CalculateAnalyteConcentration
  //Purpose: Perform a concentration calculation for a carrier gas with a particular analyte flow rate and source concentration.
  //Input:   carrierRate (double) - The carrier gas flow rate in mL/min.
  //         analyteRate (double) - The analyte flow rate in mL/hr.
  //         sourceConcentration (double) - The analyte source concentration in ppm.
  //Output:  concentration (double) - The calculated concentration in ppm.
  //Note:    Concentration units are not strictly required to be ppm as calculation is a done by a unit-less scalar.
  double CalculateAnalyteConcentration(double carrierRate, double analyteRate, double sourceConcentration);

  //Name:    CalculateSourceConcentration
  //Purpose: Perform a concentration calculation for a carrier gas with a particular analyte flow rate and diluted concentration.
  //Input:   carrierRate (double) - The carrier gas flow rate in mL/min.
  //         analyteRate (double) - The analyte flow rate in mL/hr.
  //         dilutedConcentration (double) - The diluted analyte source concentration in ppm.
  //Output:  concentration (double) - The calculated concentration in ppm.
  //Note:    Concentration units are not strictly required to be ppm as calculation is a done by a unit-less scalar.
  double CalculateSourceConcentration(double carrierRate, double analyteRate, double dilutedConcentration);
  
  //Name:    CalculateAnalyteRate
  //Purpose: Calculate the analyte rate required with provided parameters.
  //Input:   carrierRate (double) - The carrier gas flow rate in mL/min.
  //         analyteConcentration (double) - The analyte concentration in ppm.
  //         dilutedConcentration (double) - The diluted analyte concentration in ppm.
  //Output:  concentration (double) - The calculated concentration in ppm.
  //Note:    Concentration units must match, but are not strictly required to be ppm as calculation is a done by a unit-less scalar.
  double CalculateAnaylteRate(double carrierRate, double sourceConcentration, double analyteConcentration);
  
  //Name:    CalculateCarrierRate
  //Purpose: Calculate the carrier rate required with provided parameters.
  //Input:   analyteRate (double) - The analyte flow rate in mL/hr.
  //         analyteConcentration (double) - The analyte concentration in ppm.
  //         dilutedConcentration (double) - The diluted analyte concentration in ppm.
  //Output:  concentration (double) - The calculated concentration in ppm.
  //Note:    Concentration units must match, but are not strictly required to be ppm as calculation is a done by a unit-less scalar.
  double CalculateCarrierRate(double sourceConcentration, double analyteConcentration, double analyteRate);
  
  //Name:    CalculateAnalyteConcentration
  //Purpose: Perform a calculation to determine the necessary liquid volume of analyte for desired concentration. 
  //Input:   analyteVolume (double) - The analyte's total volume in Liters (L).
  //         analyteMolarMass (double) - The analyte's molar mass in grams per mol (g/mol).
  //         analyteDensity (double) - The analyte's density in grams per cubic centimeter (g/cm^3).
  //         carrierVolume (double) - The carrier's total volume in Liters (L).
  //         carrierMolarMass (double) - The carrier's molar mass in grams per mol (g/mol).
  //         carrierDensity (double) - The carrier's density in grams per cubic centimeter (g/cm^3).
  //Output:  volume (double) - The calculated concentration in parts per million (ppm).
  //Note:    Calculate analyte concentration from PPM = (1E6 * V * D * MM_c) / (V_c * D_c * MM)
  double CalculateAnalyteConcentration(double analyteVolume, double analyteMolarMass, double analyteDensity, double carrierVolume, double carrierMolarMass, double carrierDensity);
  
  //Name:    CalculateAnalyteVolume
  //Purpose: Perform a calculation to determine the necessary liquid volume of analyte for desired concentration. 
  //Input:   targetConcentration (double) - The desired concentration in parts per million (ppm).
  //         analyteMolarMass (double) - The analyte's molar mass in grams per mol (g/mol).
  //         analyteDensity (double) - The analyte's density in grams per cubic centimeter (g/cm^3).
  //         carrierVolume (double) - The carrier's total volume in Liters (L).
  //         carrierMolarMass (double) - The carrier's molar mass in grams per mol (g/mol).
  //         carrierDensity (double) - The carrier's density in grams per cubic centimeter (g/cm^3).
  //Output:  volume (double) - The calculated volume in Liters (L).
  //Note:    Calculate analyte volume from V =  V_c * (PPM / 1E6) * (D_c * MM) / (D * MM_c)
  double CalculateAnalyteVolume(double targetConcentration, double analyteMolarMass, double analyteDensity, double carrierVolume, double carrierMolarMass, double carrierDensity);
  
  //Name:    CalculateCarrierVolume
  //Purpose: Perform a calculation to determine the necessary volume of carrier gas for desired concentration and volume of analyte. 
  //Input:   targetConcentration (double) - The desired concentration in parts per million (ppm).
  //         analyteMolarMass (double) - The analyte's molar mass in grams per mol (g/mol).
  //         analyteDensity (double) - The analyte's density in grams per cubic centimeter (g/cm^3).
  //         carrierVolume (double) - The carrier's total volume in Liters (L).
  //         carrierMolarMass (double) - The carrier's molar mass in grams per mol (g/mol).
  //         carrierDensity (double) - The carrier's density in grams per cubic centimeter (g/cm^3).
  //Output:  volume (double) - The calculated volume in Liters (L).
  //Note:    Calculate analyte volume from V_c = V / ((PPM / 1E6) * (D_c * MM) / (D * MM_c))
  double CalculateCarrierVolume(double targetConcentration, double analyteMolarMass, double analyteDensity, double analyteVolume, double carrierMolarMass, double carrierDensity);
  
  //Name:    ContainsItem
  //Purpose: Template function to check presence of item in list.
  //Input:   list (std::vector<T>) - An arbitrary vector.
  //         item (T) - The target item.
  //Output:  present (bool) - Status of find operation.
  //Note:    Implementation present in header file as function will be copied to source file by compiler.
  template <typename T> bool ContainsItem(std::vector<T> list, T item){
    
    bool present = false;
    
    for(unsigned i = 0; !present && (i < list.size()); i++){
      present |= (list[i] == item);
    }
    
    return present;
  }
  
}

#endif
