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
#define DET_HIT_RATIO "Detector Hit Ratio"
#define AVG_COUNT "Average Count"

//Find the index of the provided value, return -1 if not found
bool FindIndex(std::vector<std::string> items, std::string value){
  int index = -1;

  for(unsigned i = 0; (index < 0) && (i < items.size()); i++){
    if(items[i] == value){
      index = i;
    }
  }
  
  return index < 0;
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
  cli.Add(INPUT_FILE, std::vector<std::string>{"i", "input"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The input file to process which represents the simulated electrical configurations for a single physical configuration. Final line is assumed to be a blank newline character.");
  cli.Add(OUTPUT_FILE, std::vector<std::string>{"o", "output"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The output file path.");
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //Print a message and exit if the user called for the help routine
  if(cli.Help()){
    return 0;
  }

  //Declare default output as current directory
  std::string output, input;
  bool verbose;
  
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
    CommaSeparatedValues inputFile, outputFile;

    //Open files
    inputFile.Open(input);
    outputFile.Open(output);

    //Read file contents
    inputFile.Read();
    outputFile.Read();

    //Get the input file dimensions
    Utilities::Limits inputSize = inputFile.Size();

    //Initialize mapping for all required columns
    std::map <std::string, int> columnToIndexMapping;
    columnToIndexMapping[ELECTRODE_PAIRS] = -1;
    columnToIndexMapping[X_LENGTH] = -1;
    columnToIndexMapping[Y_LENGTH] = -1;
    columnToIndexMapping[Z_LENGTH] = -1;
    columnToIndexMapping[Z_LENGTH_SIM] = -1;
    columnToIndexMapping[ION_FILE] = -1;
    columnToIndexMapping[ION_MASS] = -1;
    columnToIndexMapping[ION_CHARGE] = -1;
    columnToIndexMapping[ION_NUMBER] = -1;
    columnToIndexMapping[GAS_RATE] = -1;
    columnToIndexMapping[BIAS_VOLTAGE] = -1;
    columnToIndexMapping[SHUTTER_VOLTAGE] = -1;
    columnToIndexMapping[LONG_VOLTAGE] = -1;
    columnToIndexMapping[SHORT_VOLTAGE] = -1;
    columnToIndexMapping[ION_START_X] = -1;
    columnToIndexMapping[ION_STOP_X] = -1;
    columnToIndexMapping[ION_START_Y] = -1;
    columnToIndexMapping[ION_STOP_Y] = -1;
    columnToIndexMapping[ION_START_Z] = -1;
    columnToIndexMapping[ION_STOP_Z] = -1;
    columnToIndexMapping[DET_HIT] = -1;

    //Instantiate a list of columns not found
    std::vector<std::string> columnsNotFound;
    std::vector<std::string> columnsInFile;
    
    //Instantiate a list to hold the key 
    std::vector<std::string> ionPacketKeys;
    std::string temp;
    bool indexNotFound = false;
    
    //Instantiate a mapping to hold all collected data
    std::map<std::string, std::map<std::string, double>> ionPacketData;
    std::map<std::string, std::map<std::string, std::string>> ionPacketInputs;
    Utilities::ConvertedData result;

    //For every column in the input file
    for(unsigned i = 0; i < inputSize.Columns; i++){
      
      //For every element in the mapping
      for(auto const& it : columnToIndexMapping){		
	//If the column header value matches the key
	if(it.first == inputFile(0,i)){
	  
	  //Update index
	  columnToIndexMapping[it.first] = i;
	}
      }
      
      //If not found
      if(columnToIndexMapping[inputFile(0,i)] < 0){
	
	//Update error list
	columnsNotFound.push_back(inputFile(0,i));
      }
      else{
	//Insert header into columns list for later loops
	columnsInFile.push_back(inputFile(0,i));
      }
    }

    //If any columns were not found
    if(columnsNotFound.size() > 0){
      //Print error message
      std::cout << "Unable to find following column headers: " << std::endl;
      for(unsigned i = 0; i < columnsNotFound.size(); i++){
	std::cout << "   '" << columnsNotFound[i] << "'" << std::endl;;
      }

      //Cease all operations
      return 1;
    }

    //For every row in the input file
    for(unsigned i = 1; (i < inputSize.Rows); i++){
      //Build ion packet key string
      temp =  inputFile(i,columnToIndexMapping[ION_FILE]) + "_"
	+ inputFile(i,columnToIndexMapping[ION_MASS]) + "_"
	+ inputFile(i,columnToIndexMapping[ION_CHARGE]) + "_"
	+ inputFile(i,columnToIndexMapping[GAS_RATE]) + "_"
	+ inputFile(i,columnToIndexMapping[BIAS_VOLTAGE]) + "_"
	+ inputFile(i,columnToIndexMapping[SHUTTER_VOLTAGE]) + "_"
	+ inputFile(i,columnToIndexMapping[LONG_VOLTAGE]) + "_"
	+ inputFile(i,columnToIndexMapping[SHORT_VOLTAGE]);

      //Determine if key has been defaulted
      indexNotFound = FindIndex(ionPacketKeys, temp);
      
      //If unable to find the current key
      if(indexNotFound){
	//Save into history list
	ionPacketKeys.push_back(temp);

	//Default data values
	ionPacketData[temp][AVG_COUNT] = 0;
      }

      //Increment average count by one
      ionPacketData[temp][AVG_COUNT] += 1;
      
      //For every column in the file
      for(unsigned j = 0; j < columnsInFile.size(); j++){
	//If index was not found
	if(indexNotFound){
	  //Default numerical value to zero
	  ionPacketData[temp][columnsInFile[j]] = 0;
	  
	  //Take a copy of the current line to save necessary string parameters
	  ionPacketInputs[temp][columnsInFile[j]] = inputFile(i,j);
	}

	//Attempt to convert string value to a double
	result = Utilities::ConvertValue_Double(inputFile(i,j));
	
	//If errors resulted from conversion and not the ion file column
	if((columnsInFile[j] != ION_FILE) && result.error){
	  //Print message to error stream
	  std::cerr << "Error converting row " << i << ", column " << j << ". " << result.msg << std::endl;
	}

	//Accumulate the data value
	ionPacketData[temp][columnsInFile[j]] += result.value;
      }
    }

    //Write header line to output
    for(unsigned i = 0; i < columnsInFile.size(); i++){
      outputFile(0,i) = columnsInFile[i];
    }

    outputFile(0, columnsInFile.size()) = DET_HIT_RATIO;
    outputFile(0, columnsInFile.size()+1) = AVG_COUNT;
    
    //For all packet keys found
    for(unsigned i = 0, columnCount = columnsInFile.size(); i < ionPacketKeys.size(); i++){
      //For all columns in the input
      for(unsigned j = 0; j < columnCount; j++){	
	//If the ion file column
	if(columnsInFile[j] == ION_FILE){
	  //Output the proper string value
	  temp = ionPacketInputs[ionPacketKeys[i]][columnsInFile[j]];
	  
	}//If the detector hit column
	else if(columnsInFile[j] == DET_HIT){	  
	  //Output accumulated value
	  temp = std::to_string(ionPacketData[ionPacketKeys[i]][columnsInFile[j]]);
	  
	}
	else{
	  //Output averaged value
	  temp = std::to_string(ionPacketData[ionPacketKeys[i]][columnsInFile[j]] / ionPacketData[ionPacketKeys[i]][AVG_COUNT]);
	}

	//Update appropriate row and column value
	outputFile(i+1,j) = temp;
      }

      //Compute ion hit percentage
      outputFile(i+1, columnCount) = std::to_string(ionPacketData[ionPacketKeys[i]][DET_HIT] / ionPacketData[ionPacketKeys[i]][AVG_COUNT]);
      outputFile(i+1, columnCount+1) = std::to_string(ionPacketData[ionPacketKeys[i]][AVG_COUNT]);
      
    }

    //Write object contents to disk
    outputFile.Write();
  }
    
  return 0;
}
