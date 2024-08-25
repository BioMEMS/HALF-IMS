//Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <limits>

//HALF-IMS libraries
#include "CLIParser.h"
//#include "CommaSeparatedValues.h"

//Pre-processor Variables
#define CHEMICAL_MOLAR_MASS "chem_molar_mass"
#define CHEMICAL_DENSITY "chem_density"
#define CHEMICAL_CONCENTRATION "chem_concentration"
#define CARRIER_VOLUME "carrier_volume"
#define CARRIER_MOLAR_MASS "carrier_molar_mass"
#define CARRIER_DENSITY "carrier_density"

int main(int argc, char *argv[]){

  //Instantiate abort flag
  bool verbose = false;

  //Instantiate default values
  double carrierGasDensity = 0.00125, carrierGasMolarMass = 28.0134, carrierGasVolume = 1.2;
  //Declare user supplied and calculated values
  double chemicalDensity, chemicalMolarMass, chemicalConcentration, chemicalVolume;
  
  //Instantiate command line input parser
  Utilities::CLIParser cli;

  //Calculate flag types to be accepted
  int flagTypeOne = Utilities::CLIParser::Dash | Utilities::CLIParser::Space;
  int flagTypeTwo = Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Space;
  int flagTypeThree = Utilities::CLIParser::Dash | Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Standalone;

  //Add default flags to parser
  cli.Add(Utilities::CLIParser::DESCRIPTION, std::vector<std::string>{""}, std::vector<int>{flagTypeThree}, "A program to perform chemical concentration calculations for HALF-IMS development.\n\nAll units are assumed to be in base SI units unless otherwise stated. The defaults for this program are that the carrier gas is Nitrogen and has a volume of 1.2 Liters. When not provided proper inputs, the program performs no action and exits.");
  cli.Add(Utilities::CLIParser::HELP, std::vector<std::string>{"h", "help"}, std::vector<int>{flagTypeThree}, "Display this help message.");
  cli.Add(Utilities::CLIParser::VERBOSE, std::vector<std::string>{"v", "verbose"}, std::vector<int>{flagTypeThree}, "Trigger verbose program output.");
  
  //Add desired flags to parser
  cli.Add(CHEMICAL_MOLAR_MASS, std::vector<std::string>{"m", "chemical_molar_mass"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The molar mass of the desired chemical in grams per mole (g/mol).");
  cli.Add(CHEMICAL_DENSITY, std::vector<std::string>{"d", "chemical_density"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The density of the desired chemical in grams per cubic centimeter (g/cm^3).");
  cli.Add(CHEMICAL_CONCENTRATION, std::vector<std::string>{"c", "chemical_concentration"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The concentration of the desired chemical in parts per million (ppm).");
  cli.Add(CARRIER_VOLUME, std::vector<std::string>{"cv", "carrier_volume"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The volume of the carrier gas in liters (L). Default is 1.2.");
  cli.Add(CARRIER_MOLAR_MASS, std::vector<std::string>{"cm", "carrier_gas_molar_mass"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The molar mass of the carrier gas in grams per mole (g/mol). Default is 0.00124.");
  cli.Add(CARRIER_DENSITY, std::vector<std::string>{"cd", "carrier_gas_density"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The molar mass of the carrier gas grams per cubic centimeter (g/cm^3). Default is 28.0134.");
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //Determine if help was printed
  if(cli.Help()){
    //Exit if printed
    return 0;
  }

  //Set verbosity flag
  verbose = cli.Present(Utilities::CLIParser::VERBOSE);
  
  //If chemical cocentration, density, and molar mass are provided
  if(cli.Present(CHEMICAL_CONCENTRATION) && cli.Present(CHEMICAL_DENSITY) && cli.Present(CHEMICAL_MOLAR_MASS)){    
    //Get the user-supplied numeric values
    chemicalConcentration = cli.GetNumeric(CHEMICAL_CONCENTRATION);
    chemicalDensity = cli.GetNumeric(CHEMICAL_DENSITY);
    chemicalMolarMass = cli.GetNumeric(CHEMICAL_MOLAR_MASS);

    //Update carrier gas volume value if provided
    if(cli.Present(CARRIER_VOLUME)){
      carrierGasVolume = cli.GetNumeric(CARRIER_VOLUME);
    }

    //Update carrier gas density value if provided
    if(cli.Present(CARRIER_VOLUME)){
      carrierGasVolume = cli.GetNumeric(CARRIER_VOLUME);
    }

    //Update carrier gas molar mass value if provided
    if(cli.Present(CARRIER_VOLUME)){
      carrierGasVolume = cli.GetNumeric(CARRIER_VOLUME);
    }
    
    //Calculate chemical volume from V = (PPM/1E6) * (D_c * MM) / (D * MM_c) * V_c
    chemicalVolume = (chemicalConcentration / 1000000.0);
    chemicalVolume *= (carrierGasDensity * chemicalMolarMass) / (chemicalDensity * carrierGasMolarMass);
    chemicalVolume *= carrierGasVolume;
    
    //If verbose
    if(verbose){
      //Print more information
      std::cout << "Carrier Gas Density (g/cm^3): " << carrierGasDensity << std::endl;
      std::cout << "Carrier Gas Molar Mass (g/mol): " << carrierGasMolarMass << std::endl;
      std::cout << "Carrier Gas Volume (L): " << carrierGasVolume << std::endl;
      std::cout << "Chemical Density (g/cm^3): " << chemicalDensity << std::endl;
      std::cout << "Chemical Molar Mass (g/mol): " << chemicalMolarMass << std::endl;
      std::cout << "Target Concentration (ppm): " << chemicalConcentration << std::endl;
      std::cout << "Chemical Volume (L): " << chemicalVolume << std::endl;
    }
    //Otherwise
    else{
      //Simply print calculated output
      std::cout << chemicalVolume << std::endl;
    }
  }
  
  return 0;
}
