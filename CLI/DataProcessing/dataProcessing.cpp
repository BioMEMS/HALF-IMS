//Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <limits>

//HALF-IMS libraries
#include "Filter.h"
#include "CLIParser.h"
#include "CommaSeparatedValues.h"
#include "Checker.h"
#include "gnuplot-iostream.h"

//Pre-processor Variables
#define OUTPUT_DIRECTORY "output"
#define INPUT_FILE "input"
#define HELP_NAME "help"

int main(int argc, char *argv[]){

  //Instantiate abort flag
  bool abort = false;

  //Instantiate command line input parser
  Utilities::CLIParser cli;

  //Calculate flag types to be accepted
  int flagTypeOne = Utilities::CLIParser::Dash | Utilities::CLIParser::Space;
  int flagTypeTwo = Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Space;
  int flagTypeThree = Utilities::CLIParser::Dash | Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Standalone;

  //Add desired flags to parser
  //cli.Add(Utilities::CLIParser::description, std::vector<std::string>{""}, std::vector<int>{flagTypeThree}, "A simple program which can take in multiple CSV files and identify the peak values.");
  cli.Add(Utilities::CLIParser::HELP, std::vector<std::string>{"h", "help"}, std::vector<int>{flagTypeThree}, "Display this help message.");
  cli.Add(Utilities::CLIParser::VERBOSE, std::vector<std::string>{"v", "verbose"}, std::vector<int>{flagTypeThree}, "Trigger verbose program output.");
  cli.Add(INPUT_FILE, std::vector<std::string>{"i", "input"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The input CSV file to process.");
  cli.Add(OUTPUT_DIRECTORY, std::vector<std::string>{"o", "output"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The output directory where to place the processed files.");
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //Print a message and exit if the user called for the help routine
  if(cli.Help()){
    return 0;
  }

  //Determine output verbosity
  bool verbose = cli.Present(Utilities::CLIParser::VERBOSE);
  
  //Declare default output as current directory
  std::string input, output = "."; //Might break on Windows? Use Filesystem library?
  //If the output flag is present
  if(cli.Present(OUTPUT_DIRECTORY)){
    //Extract necessary arguments
    output = cli.Get(OUTPUT_DIRECTORY);
  }
  
  //Get the input file to process
  if(cli.Present(INPUT_FILE)){
    input = cli.Get(INPUT_FILE);
  }
  else{
    std::cerr << "Inputs are required." << std::endl;
    abort = true;
  }

  //Determine if the input file exists
  if(!std::filesystem::exists(std::filesystem::path(input)) || std::filesystem::is_directory(std::filesystem::path(input))){
    std::cerr << "Invalid input file: '" << output << "'" << std::endl;
    abort |= true;
  }
  
  //Determine if the output directory exists
  if(!std::filesystem::exists(std::filesystem::path(output)) || !std::filesystem::is_directory(std::filesystem::path(output))){
    std::cerr << "Invalid output directory: '" << output << "'" << std::endl;
    abort |= true;
  }

  
  //If abort triggered
  if(abort){
    //Stop
    return 1;
  }

  //Open input file
  CommaSeparatedValues inputFile;
  inputFile.Open(input);
  inputFile.Read();

  //Get input file size
  Utilities::Limits inputSize = inputFile.Size();
  
  //Store a mapping of column header to index
  std::map <std::string, unsigned> columnToIndexMapping;
  for(unsigned i = 0; i < inputSize.Columns; i++){
    columnToIndexMapping[inputFile(0, i)] = i;

    //If the value was too large for integer value
    if(((int)columnToIndexMapping[inputFile(0, i)]) < 0){
      //Reset value to zero to avoid improper indexing
      columnToIndexMapping[inputFile(0, i)] = 0;
      std::cerr << "Column index '" << i << "' exceeds program capabilities. Index set to '" << columnToIndexMapping[inputFile(0, i)] << "' to prevent errors." << std::endl;
    }
  }

  //Gather list of unique values for each column
  std::map <std::string, std::vector<std::string>> columnUniqueValues;
  for(unsigned i = 0; i < inputSize.Columns; i++){
    for(unsigned j = 1; j < inputSize.Rows; j++){
      //If the column list does not contain the item
      if(!Utilities::ContainsItem(columnUniqueValues[inputFile(0,i)], inputFile(j, i))){
	//Save it to the list
	columnUniqueValues[inputFile(0,i)].push_back(inputFile(j, i));
      }
    }
  }

  //Print out found values
  if(verbose){
    std::cout << "Discovered Unique Values" << std::endl;
    //For every element in the mapping
    for(auto const& it : columnUniqueValues){		      
      //Print out column header
      std::cout << it.first << ": ";
      
      //Print out all found values in a comma-separated list
      for(unsigned i = 0, valueCount = columnUniqueValues[it.first].size(); i < valueCount; i++){
	std::cout << columnUniqueValues[it.first][i];

	//If value is not the last
	if((i + 1) < valueCount){
	  std::cout << ", ";
	}
      }
      std::cout << std::endl;
    }

    std::cout << std::endl;
  }

  //Build a sub-title string for the generated plots
  std::string constantParameters = "";
  for(auto const& it : columnUniqueValues){
    if(columnUniqueValues[it.first].size() == 1){
      //If not the first element
      if(constantParameters.length() > 0){
	//Separate values with a space
	constantParameters += " ";
      }

      //Append the column header and unique value in a comma-separated list
      constantParameters += it.first + ": " + columnUniqueValues[it.first][0] + ",";
    }
  }
  
  //Remove trailing comma
  constantParameters = constantParameters.substr(0, constantParameters.length() - 1);

  //Print out header line
  if(verbose){
    std::cout << "Plot Subtitle" << std::endl << constantParameters << std::endl << std::endl;
  }

  //For every column in the input
  for(auto const& xColumn : columnUniqueValues){
    //If the column has more than one value
    if(columnUniqueValues[xColumn.first].length() > 1){
      //Partition values for 2D plot

      //Partition values for 3D plot
    }
  }
  
  return 0;
}
