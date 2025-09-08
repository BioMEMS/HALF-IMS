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
#define DEBUG "debug"

//Pre-processor Variables for file column headers
#define GROUPED_FLAG "Grouped Flag"
#define REPULSION_SETT "Repulsion Setting"
#define REPULSION_VAL "Repulsion Value"
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
#define GRID_X "Grid X Spacing (mm)"
#define GRID_Y "Grid Y Spacing (mm)"
#define GRID_Z "Grid Z Spacing (mm)"
#define ELECTRODE_HEIGHT "Electrode Height (mm)"
#define SHUTTER_LENGTH "Shutter Length (mm)"
#define FIRST_GROUND "First Ground Length (mm)"
#define SECOND_GROUND "Second Ground Length (mm)"
#define LONG_LENGTH "Long Electrode Length (mm)"
#define SHORT_LENGTH "Short Electrode Length (mm)"
#define BIAS_LENGTH "Bias Length (mm)"
#define DETECTOR_LENGTH "Detector Length (mm)"
#define CHEM_CONCENTRATION "Concentration (ppm)"
#define ION_VELOCITY_X "Ion X Velocity (mm/usec)"
#define ION_VELOCITY_Y "Ion Y Velocity (mm/usec)"
#define ION_VELOCITY_Z "Ion Z Velocity (mm/usec)"
#define UPSTREAM_PRESSURE "Upstream Pressure (psi)"
#define CARRIER_PRESSURE "Carrier Pressure (psi)"

//Pre-processor variables for output file columns
#define AVG_COUNT "Average Count"
#define ION_CURRENT "Ion Current (pA)"
#define CONTROL_RATIO "Control Ratio (V/V)"

//Pre-processor variables for value processing
#define ROUNDING_DIGITS 5

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
  cli.Add(DEBUG, std::vector<std::string>{"d", "debug"}, std::vector<int>{flagTypeThree}, "Trigger verbose debug program output.");
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //Print a message and exit if the user called for the help routine
  if(cli.Help()){
    return 0;
  }

  //Declare default output as current directory
  std::string output, input;
  bool verbose, debug;
  
  //Determine output verbosity
  debug = cli.Present(DEBUG);
  verbose = debug || cli.Present(Utilities::CLIParser::VERBOSE);
  
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
    columnToIndexMapping[GRID_X] = -1;
    columnToIndexMapping[GRID_Y] = -1;
    columnToIndexMapping[GRID_Z] = -1;
    columnToIndexMapping[ELECTRODE_HEIGHT] = -1;
    columnToIndexMapping[SHUTTER_LENGTH] = -1;
    columnToIndexMapping[FIRST_GROUND] = -1;
    columnToIndexMapping[SECOND_GROUND] = -1;
    columnToIndexMapping[LONG_LENGTH] = -1;
    columnToIndexMapping[SHORT_LENGTH] = -1;
    columnToIndexMapping[BIAS_LENGTH] = -1;
    columnToIndexMapping[DETECTOR_LENGTH] = -1;
    columnToIndexMapping[CHEM_CONCENTRATION] = -1;
    columnToIndexMapping[GROUPED_FLAG] = -1;
    columnToIndexMapping[REPULSION_SETT] = -1;
    columnToIndexMapping[REPULSION_VAL] = -1;
    columnToIndexMapping[ION_VELOCITY_X] = -1;
    columnToIndexMapping[ION_VELOCITY_Y] = -1;
    columnToIndexMapping[ION_VELOCITY_Z] = -1;
    columnToIndexMapping[UPSTREAM_PRESSURE] = -1;
    columnToIndexMapping[CARRIER_PRESSURE] = -1;

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

      if(debug){
	std::cout << inputFile(0,i) << ",";
      }
    }

    if(debug){
      std::cout << std::endl;
    }
    
    //If any columns were not found
    if(columnsNotFound.size() > 0){
      //Print error message
      std::cerr << "Unable to find following column headers: " << std::endl;
      for(unsigned i = 0; i < columnsNotFound.size(); i++){
	std::cerr << "   '" << columnsNotFound[i] << "'" << std::endl;;
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

	if(debug){
	  std::cout << inputFile(i,j) << ",";
	}
	
	//If errors resulted from conversion and not the ion file column
	if((columnsInFile[j] != ION_FILE) && (columnsInFile[j] != REPULSION_SETT) && result.error){
	  //Print message to error stream
	  std::cerr << "Error converting row " << i << ", column " << j << ". " << result.msg << std::endl;
	}

	//Accumulate the data value
	ionPacketData[temp][columnsInFile[j]] += result.value;
      }

      if(debug){
	std::cout << std::endl;
      }
    }

    //Write header line to output
    for(unsigned i = 0; i < columnsInFile.size(); i++){
      if(verbose){
	std::cout << "Setting column " << i + 1 << " header as '" << columnsInFile[i] << "' in the output file." << std::endl;
      }
      outputFile(0,i) = columnsInFile[i];
    }

    //Add calculated columns to mapping
    columnToIndexMapping[DET_HIT_RATIO] = columnsInFile.size();
    columnToIndexMapping[AVG_COUNT] = columnsInFile.size() + 1;
    columnToIndexMapping[ION_CURRENT] = columnsInFile.size() + 2;
    columnToIndexMapping[CONTROL_RATIO] = columnsInFile.size() + 3;
    
    //Write the appropriate columns headers
    outputFile(0, columnToIndexMapping[DET_HIT_RATIO]) = DET_HIT_RATIO;
    outputFile(0, columnToIndexMapping[AVG_COUNT]) = AVG_COUNT;
    outputFile(0, columnToIndexMapping[ION_CURRENT]) = ION_CURRENT;
    outputFile(0, columnToIndexMapping[CONTROL_RATIO]) = CONTROL_RATIO;
    
    //For all packet keys found
    for(unsigned i = 0, fileRow = 1, columnCount = columnsInFile.size(); i < ionPacketKeys.size(); i++, fileRow = i + 1){
      //For all columns in the input
      for(unsigned j = 0; j < columnCount; j++){	
	//If the ion file column
	if((columnsInFile[j] == ION_FILE) || (columnsInFile[j] == REPULSION_SETT)){
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
	outputFile(fileRow,j) = temp;
      }

      //Compute ion hit percentage
      outputFile(fileRow, columnToIndexMapping[DET_HIT_RATIO]) = std::to_string(ionPacketData[ionPacketKeys[i]][DET_HIT] / ionPacketData[ionPacketKeys[i]][AVG_COUNT]);
      outputFile(fileRow, columnToIndexMapping[AVG_COUNT]) = std::to_string(ionPacketData[ionPacketKeys[i]][AVG_COUNT]);      
      outputFile(fileRow, columnToIndexMapping[ION_CURRENT]) = std::to_string((1E12) * Utilities::CalculateCurrent(ionPacketData[ionPacketKeys[i]][DET_HIT], ionPacketData[ionPacketKeys[i]][X_LENGTH] / ((1E6) * ionPacketData[ionPacketKeys[i]][ION_VELOCITY_X])));
      outputFile(fileRow, columnToIndexMapping[CONTROL_RATIO]) = std::to_string(Utilities::TruncateValue(Utilities::CalculateNormalizedControl(ionPacketData[ionPacketKeys[i]][LONG_VOLTAGE], ionPacketData[ionPacketKeys[i]][SHORT_VOLTAGE]), ROUNDING_DIGITS));
    }

    //Write object contents to disk
    outputFile.Write();
  }
    
  return 0;
}
