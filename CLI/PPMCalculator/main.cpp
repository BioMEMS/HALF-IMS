//Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <limits>

//HALF-IMS libraries
#include "CLIParser.h"
#include "Utilities.h"
//#include "CommaSeparatedValues.h"

//Pre-processor variables for CLI parser names
#define ANALYTE_MOLAR_MASS "chem_molar_mass"
#define ANALYTE_DENSITY "chem_density"
#define ANALYTE_CONCENTRATION "chem_concentration"
#define ANALYTE_VOLUME "chem_volume"
#define CARRIER_MOLAR_MASS "carrier_molar_mass"
#define CARRIER_DENSITY "carrier_density"
#define CARRIER_VOLUME "carrier_volume"
#define MODULE_CALC_BAG "ppm_calc_volume"
#define MODULE_CALC_PUMP "ppm_calc_pump"
#define CARRIER_FLOW_RATE "carrier_rate"
#define PUMP_FLOW_RATE "pump_rate"
#define REVERSE_CALCULATION "reverse_calculation"

//Pre-processor varaibles for CLI parser flag types
#define STANDALONE_FLAG Utilities::CLIParser::Dash | Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Standalone
#define DASH_STANDALONE_FLAG Utilities::CLIParser::Dash | Utilities::CLIParser::Standalone
#define DASH_DASH_STANDALONE_FLAG Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Standalone
#define DASH_SPACE_FLAG Utilities::CLIParser::Dash | Utilities::CLIParser::Space
#define DASH_DASH_SPACE_FLAG Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Space

enum ReturnCodes{
  SUCCESS=0,
  DEFAULT,
  BAG,
  PUMP
};

//Main function
int main(int argc, char *argv[]){

  //Instantiate abort flag
  bool verbose = false;

  //Instantiate return code
  int returnCode = 1;
  
  //Instantiate default values
  double carrierGasDensity = 0.00125, carrierGasMolarMass = 28.0134, carrierGasVolume = 1.2, carrierGasRate = 1200;
  //Declare user supplied and calculated values
  double analyteDensity, analyteMolarMass, analyteConcentration, analyteVolume, pumpRate, result;
  
  //Instantiate command line input parser
  Utilities::CLIParser cli;

  //Add default flags to parser
  cli.Add(Utilities::CLIParser::DESCRIPTION, std::vector<std::string>{""}, std::vector<int>{STANDALONE_FLAG}, "A program to perform chemical concentration calculations for HALF-IMS development.\n\nAll units are assumed to be in base SI units unless otherwise stated. The defaults for this program are that the carrier gas is Nitrogen and has a volume of 1.2 Liters. When not provided proper inputs, the program performs no action and exits.");
  cli.Add(Utilities::CLIParser::HELP, std::vector<std::string>{"h", "help"}, std::vector<int>{STANDALONE_FLAG}, "Display this help message.");
  cli.Add(Utilities::CLIParser::VERBOSE, std::vector<std::string>{"v", "verbose"}, std::vector<int>{STANDALONE_FLAG}, "Trigger verbose program output.");

  //Add desired module flags to parser
  cli.Add(MODULE_CALC_BAG, std::vector<std::string>{"bag"}, std::vector<int>{DASH_STANDALONE_FLAG}, "Invoke the bag calculation submodule.");
  cli.Add(MODULE_CALC_PUMP, std::vector<std::string>{"pump"}, std::vector<int>{DASH_STANDALONE_FLAG}, "Invoke the pump calculation submodule.");
  
  //Add desired flags to parser
  cli.Add(ANALYTE_VOLUME, std::vector<std::string>{"av", "analyte_volume"}, std::vector<int>{DASH_SPACE_FLAG, DASH_DASH_SPACE_FLAG}, "The volume of the analyte in liters (L).");
  cli.Add(ANALYTE_MOLAR_MASS, std::vector<std::string>{"am", "analyte_molar_mass"}, std::vector<int>{DASH_SPACE_FLAG, DASH_DASH_SPACE_FLAG}, "The molar mass of the desired analyte in grams per mole (g/mol).");
  cli.Add(ANALYTE_DENSITY, std::vector<std::string>{"ad", "analyte_density"}, std::vector<int>{DASH_SPACE_FLAG, DASH_DASH_SPACE_FLAG}, "The density of the desired analyte in grams per cubic centimeter (g/cm^3).");
  cli.Add(ANALYTE_CONCENTRATION, std::vector<std::string>{"ac", "analyte_concentration"}, std::vector<int>{DASH_SPACE_FLAG, DASH_DASH_SPACE_FLAG}, "The concentration of the desired analyte in parts per million (ppm).");
  cli.Add(CARRIER_VOLUME, std::vector<std::string>{"cv", "carrier_volume"}, std::vector<int>{DASH_SPACE_FLAG, DASH_DASH_SPACE_FLAG}, "The volume of the carrier gas in liters (L). Default is 1.2.");
  cli.Add(CARRIER_MOLAR_MASS, std::vector<std::string>{"cm", "carrier_gas_molar_mass"}, std::vector<int>{DASH_SPACE_FLAG, DASH_DASH_SPACE_FLAG}, "The molar mass of the carrier gas in grams per mole (g/mol). Default is 0.00124.");
  cli.Add(CARRIER_DENSITY, std::vector<std::string>{"cd", "carrier_gas_density"}, std::vector<int>{DASH_SPACE_FLAG, DASH_DASH_SPACE_FLAG}, "The molar mass of the carrier gas in grams per cubic centimeter (g/cm^3). Default is 28.0134.");
  cli.Add(CARRIER_FLOW_RATE, std::vector<std::string>{"cr", "carrier_flow_rate"}, std::vector<int>{DASH_SPACE_FLAG, DASH_DASH_SPACE_FLAG}, "The carrier gas flow rate in milliliters per minute (mL/min). Default is 1200.");
  cli.Add(PUMP_FLOW_RATE, std::vector<std::string>{"pr", "pump_flow_rate"}, std::vector<int>{DASH_SPACE_FLAG, DASH_DASH_SPACE_FLAG}, "The analyte flow rate in milliliters per hour (mL/hr).");
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //Determine if help was printed
  if(cli.Help()){
    //Exit if printed
    return 0;
  }

  //Set verbosity flag
  verbose = cli.Present(Utilities::CLIParser::VERBOSE);
  
  //If the user selected the bag calculation module
  if(cli.Present(MODULE_CALC_BAG)){
    //Set bag failure code
    returnCode = ReturnCodes::BAG;
    
    //If analyte concentration, density, and molar mass are provided
    if(cli.Present(ANALYTE_DENSITY) && cli.Present(ANALYTE_MOLAR_MASS)){
      //Get the user-supplied numeric values
      analyteDensity = cli.GetNumeric(ANALYTE_DENSITY);
      analyteMolarMass = cli.GetNumeric(ANALYTE_MOLAR_MASS);

      //Update analyte volume value if provided
      if(cli.Present(ANALYTE_VOLUME)){
	analyteVolume = cli.GetNumeric(ANALYTE_VOLUME);
      }
      
      //Update analyte concentration value if provided
      if(cli.Present(ANALYTE_CONCENTRATION)){
	analyteConcentration = cli.GetNumeric(ANALYTE_CONCENTRATION);
      }
      
      //Update carrier gas volume value if provided
      if(cli.Present(CARRIER_VOLUME)){
	carrierGasVolume = cli.GetNumeric(CARRIER_VOLUME);
      }
      
      //Update carrier gas density value if provided
      if(cli.Present(CARRIER_DENSITY)){
	carrierGasDensity = cli.GetNumeric(CARRIER_DENSITY);
      }
      
      //Update carrier gas molar mass value if provided
      if(cli.Present(CARRIER_MOLAR_MASS)){
	carrierGasMolarMass = cli.GetNumeric(CARRIER_MOLAR_MASS);
      }

      //If the analyte concentration and volume values were provided
      if(cli.Present(ANALYTE_CONCENTRATION) && cli.Present(ANALYTE_VOLUME)){
	//Calculate the carrier gas volume required to achieve 
	result = Utilities::CalculateCarrierVolume(analyteConcentration, analyteMolarMass, analyteDensity, analyteVolume, carrierGasMolarMass, carrierGasDensity);
	carrierGasVolume = result;

	returnCode = ReturnCodes::SUCCESS;
      }
      //If the analyte volume is provided
      else if(cli.Present(ANALYTE_VOLUME)){
	//Calculate the analyte concentration
	result = Utilities::CalculateAnalyteConcentration(analyteVolume, analyteMolarMass, analyteDensity, carrierGasVolume, carrierGasMolarMass, carrierGasDensity);
        analyteConcentration = result;

	returnCode = ReturnCodes::SUCCESS;
      }
      //If the analyte concentration is provided
      else if(cli.Present(ANALYTE_CONCENTRATION)){
	//Perform analyte volume calculation
	result = Utilities::CalculateAnalyteVolume(analyteConcentration, analyteMolarMass, analyteDensity, carrierGasVolume, carrierGasMolarMass, carrierGasDensity);
	analyteVolume = result;

	returnCode = ReturnCodes::SUCCESS;
      }
      
    }
    
    //If operation succeeded
    if(returnCode == ReturnCodes::SUCCESS){
      //If verbose
      if(verbose){
	//Print more information
	std::cout << "Carrier Gas Density (g/cm^3): " << carrierGasDensity << std::endl;
	std::cout << "Carrier Gas Molar Mass (g/mol): " << carrierGasMolarMass << std::endl;
	std::cout << "Carrier Gas Volume (L): " << carrierGasVolume << std::endl;
	std::cout << "Analyte Density (g/cm^3): " << analyteDensity << std::endl;
	std::cout << "Analyte Molar Mass (g/mol): " << analyteMolarMass << std::endl;
	std::cout << "Analyte Concentration (ppm): " << analyteConcentration << std::endl;
	std::cout << "Analyte Volume (L): " << analyteVolume << std::endl;
      }
      //Otherwise
      else{
	//Simply print calculated output
	std::cout << result << std::endl;
      }
    }
  }
  //If user selected pump calculation module
  else if (cli.Present(MODULE_CALC_PUMP)){
    //Set pump failure code
    returnCode = ReturnCodes::PUMP;
    
    //If user provided a carrier gas rate
    if(cli.Present(CARRIER_FLOW_RATE)){
      //Update rate
      carrierGasRate = cli.GetNumeric(CARRIER_FLOW_RATE);
    }

    //If pump rate and analyte concentration are provided
    if(cli.Present(PUMP_FLOW_RATE) && cli.Present(ANALYTE_CONCENTRATION)){
      //Get user provided values
      analyteConcentration = cli.GetNumeric(ANALYTE_CONCENTRATION);
      pumpRate = cli.GetNumeric(PUMP_FLOW_RATE);

      //Calculate the analyte concentration in the system
      result = Utilities::CalculateAnalyteConcentration(carrierGasRate, pumpRate, analyteConcentration);

      returnCode = ReturnCodes::SUCCESS;
    }

    //If operation succeeded
    if(returnCode == ReturnCodes::SUCCESS){
      //If verbose
      if(verbose){
	//Print all parameters
	std::cout << "Carrier Gas Rate (mL/min): " << carrierGasRate << std::endl;
	std::cout << "Analyte Rate (mL/hr): " << pumpRate << std::endl;
	std::cout << "Source Concentration (ppm): " << analyteConcentration << std::endl;
	std::cout << "Diluted Concentration (ppm): " << result << std::endl;
      }
      else{
	//Print only calculated result
	std::cout << result << std::endl;
      }
    }
  }
  
  return returnCode;
}
