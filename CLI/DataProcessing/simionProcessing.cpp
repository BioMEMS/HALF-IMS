//Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <limits>

//HALF-IMS libraries
#include "CLIParser.h"
#include "CommaSeparatedValues.h"
#include "Checker.h"

//Pre-processor Variables
#define OUTPUT_FILE "output"
#define INPUT_FILE "input"

//Pre-processor Variables for file column headers
#define ELECTRODE_PAIRS "Electrode Pairs"
#define X_LENGTH "Device X Length (mm)"
#define Y_LENGTH "Device Y Length (mm)"
#define Z_LENGTH "Device Z Length (mm)"
#define Z_LENGTH_SIM "Device Z Length Simulated (mm)"
#define ION_FILE "Current Ion File"
#define ION_MASS "Ion Mass (u)"
#define ION_CHARGE "Ion Charge (e)"
#define ION_NUMBER "Ion Number"
#define GAS_RATE "Carrier Gas Flow Rate (mL/min)"
#define BIAS_VOLTAGE "Bias Ring (V)"
#define SHUTTER_VOLTAGE "Shutter Electrode (V)"
#define LONG_VOLTAGE "Long Electrode (V)"
#define SHORT_VOLTAGE "Short Electrode (V)"
#define ION_START_X "Initial Ion X Position (mm)"
#define ION_STOP_X "Final Ion X Position (mm)"
#define ION_START_Y "Initial Ion Y Position (mm)"
#define ION_STOP_Y "Final Ion Y Position (mm)"
#define ION_START_Z "Initial Ion Z Position (mm)"
#define ION_STOP_Z "Final Ion Z Position (mm)"
#define DET_HIT "Detector Pad Hit"

//Convert provided vect
std::vector<double> ConvertLine(unsigned lineNumber, std::vector<std::string> line, unsigned chemicalIndex){
  std::vector<double> results;

  //For each element in the line
  for(unsigned i = 0; i < line.size(); i++){
    //Attempt to convert element to a double
    try{
      results.push_back(std::stod(line[i]));
    }
    catch(std::invalid_argument const& ex){
      //If the current index is not the known chemical index
      if(i != chemicalIndex){
	//Print message to error stream
	std::cerr << "Exception in '" << ex.what() << "' thrown attempting to convert '" << line[i] << "' at position " << i << " for line " << lineNumber << ". ";
	std::cerr << "A minimum data value has been added as a placeholder to preserve any data spacing." << std::endl;
      }
      
      //Push minimum double value
      results.push_back(std::numeric_limits<double>::min());
    }
  }

  return results;
}

int main(int argc, char *argv[]){

  //Instantiate command line input parser
  Utilities::CLIParser cli;

  //Calculate flag types to be accepted
  int flagTypeOne = Utilities::CLIParser::Dash | Utilities::CLIParser::Space;
  int flagTypeTwo = Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Space;
  int flagTypeThree = Utilities::CLIParser::Dash | Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Standalone;

  //Add desired flags to parser
  cli.Add(Utilities::CLIParser::DESCRIPTION, std::vector<std::string>{""}, std::vector<int>{flagTypeThree}, "A program to process SIMION output CSV file into something that can be processed by GNU Plot for HALF-IMS development. When not provided proper inputs, the program performs no action and exits.");
  cli.Add(Utilities::CLIParser::HELP, std::vector<std::string>{"h", "help"}, std::vector<int>{flagTypeThree}, "Display this help message.");
  cli.Add(Utilities::CLIParser::VERBOSE, std::vector<std::string>{"v", "verbose"}, std::vector<int>{flagTypeThree}, "Trigger verbose program output.");

  //Add user flags
  cli.Add(INPUT_FILE, std::vector<std::string>{"i", "input"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The input file to process. Final line is assumed to be a blank newline character.");
  cli.Add(OUTPUT_FILE, std::vector<std::string>{"o", "output"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The output file path.");
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //Print a message and exit if the user called for the help routine
  if(cli.Help()){
    return 0;
  }

  //Declare default output as current directory
  std::string output, input;
  bool inputFlag, outputFlag, verbose;
  
  //Determine output verbosity
  verbose = cli.Present(Utilities::CLIParser::VERBOSE);
  
  //If the necessary inputs are present
  if(cli.Present(OUTPUT_FILE) && cli.Present(INPUT_FILE)){
    //Extract necessary arguments
    input = cli.Get(INPUT_FILE);
    output = cli.Get(OUTPUT_FILE);

    //If verbose
    if(verbose){
      //Report input and output file
      std::cout << "Input File: '" << input << "'" << std::endl;
      std::cout << "Output File: '" << output << "'" <<  std::endl;
    }

    //Create file streams for input and output
    std::fstream inputFile, outputFile;

    //Open files
    inputFile.open(input, std::fstream::in);
    outputFile.open(output, std::fstream::out | std::fstream::trunc);

    //Determine if either file is open
    inputFlag = inputFile.is_open();
    outputFlag = outputFile.is_open();
    
    //If both file streams were opened
    if(inputFlag && outputFlag){

    }    
  }
    
  return 0;
}
