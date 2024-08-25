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
#define CARRIER_VOLUME "carrier_volume"
#define CARRIER_MOLAR_MASS "carrier_molar_mass"
#define CARRIER_DENSITY "carrier_density"

int main(int argc, char *argv[]){

  //Instantiate abort flag
  bool abort = false;

  //Instantiate default filtering values
  double aperture = 20;
  unsigned filterRepeats = 1;
  
  //Instantiate command line input parser
  Utilities::CLIParser cli;

  //Calculate flag types to be accepted
  int flagTypeOne = Utilities::CLIParser::Dash | Utilities::CLIParser::Space;
  int flagTypeTwo = Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Space;
  int flagTypeThree = Utilities::CLIParser::Dash | Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Standalone;

  //Add desired flags to parser
  cli.Add(Utilities::CLIParser::description, std::vector<std::string>{""}, std::vector<int>{flagTypeThree}, "A program to perform chemical concentration calculations for HALF-IMS development. The defaults for this program are that the carrier gas is Nitrogen and has a volume of 1.2 Liters.");
  cli.Add(Utilities::CLIParser::help, std::vector<std::string>{"h", "help"}, std::vector<int>{flagTypeThree}, "Display this help message.");
  cli.Add(CHEMICAL_MOLAR_MASS, std::vector<std::string>{"m", "chemical_molar_mass"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The molar mass of the desired chemical in grams per mole (g/mol).");
  cli.Add(CHEMICAL_DENSITY, std::vector<std::string>{"d", "chemical_density"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The density of the desired chemical in grams per cubic centimeter (g/cm^3).");
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
  
  return 0;
}
