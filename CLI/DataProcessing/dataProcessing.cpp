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
  cli.Add(Utilities::CLIParser::DESCRIPTION, std::vector<std::string>{""}, std::vector<int>{flagTypeThree}, "A program which can process HALF-IMS CSV files into plots using GnuPlot.");
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
    std::cerr << "Invalid input file: '" << input << "'" << std::endl;
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

  std::vector<std::vector<std::string>> possiblePlots;
  std::map<std::string, std::vector<std::tuple<double, double>>> plotData;
  std::map<std::string, std::vector<std::vector<std::string>>> plotNameParameters;
  std::vector<std::vector<std::tuple<double, double>>> curveData;
  std::vector<std::string> curveLabel;
  Utilities::ConvertedData xResult, yResult, zResult;
  unsigned curveCount = 0;

  //For every column in the input file
  for(auto const& xColumn : columnUniqueValues){

    //If column has more than one value
    if(columnUniqueValues[xColumn.first].size() > 1){

      //Attempt to pair with every other possible column
      for(auto const& yColumn : columnUniqueValues){
	
	//So long as there is more than one value in the column and it is not the X-Column
	if((columnUniqueValues[yColumn.first].size() > 1) && (xColumn.first != yColumn.first)){

	  //Add a two dimensional plot to the list
	  possiblePlots.push_back(std::vector<std::string> { xColumn.first, yColumn.first });

	  //Attempt to pair X- and Y-columns with every other possible column
	  for(auto const& zColumn : columnUniqueValues){
	    //So long as there is more than one value in the column and it is not the X- or Y-Columns
	    if((columnUniqueValues[zColumn.first].size() > 1) && (xColumn.first != zColumn.first) && (yColumn.first != zColumn.first)){
	      
	      //Add a three dimensional plot to the list
	      possiblePlots.push_back(std::vector<std::string> {xColumn.first, yColumn.first, zColumn.first});
	    }
	  }
	}
      }
    }
  }

  //For every possible plot
  for(unsigned plotId = 0, xCol=0, yCol=0, zCol=0; plotId < possiblePlots.size(); plotId++){
    //Clear all previous values
    plotData.clear();
    curveData.clear();
    plotNameParameters.clear();
 
    //Convert string to numerical index of known columns
    xCol = columnToIndexMapping[possiblePlots[plotId][0]];
    yCol = columnToIndexMapping[possiblePlots[plotId][1]];

    //If more than two dimensions
    if(possiblePlots[plotId].size() > 2){
      //Convert third column
      zCol = columnToIndexMapping[possiblePlots[plotId][2]];
    }
    else{
      //Match column to second as an error condition
      zCol = yCol;
    }
    
    for(unsigned i = 1; i < inputSize.Rows; i++){
      xResult = Utilities::ConvertValue_Double(inputFile(i, xCol));
      yResult = Utilities::ConvertValue_Double(inputFile(i, yCol));
      zResult = Utilities::ConvertValue_Double(inputFile(i, zCol));
      
      //If errors did not result
      if(!xResult.error && !yResult.error && !zResult.error){
	//If more than 
	if(zCol != yCol){
	  //
	}
	else{
	  //Add data to appropriate list
	  plotData[inputFile(i, xCol)].push_back(std::make_tuple(xResult.value, yResult.value));
	  
	  //Add blank list of parameters
	  plotNameParameters[inputFile(i, xCol)].push_back(std::vector<std::string>{});
	  
	  //Copy values in columns
	  for(unsigned j = 0, currentVector = plotNameParameters[inputFile(i, xCol)].size() - 1; j < inputSize.Columns; j++){
	    //If value changes within file
	    if((j != xCol) && (j != yCol) && (columnUniqueValues[inputFile(0, j)].size() > 1)){
	      //Add it to the list for curve names
	      plotNameParameters[inputFile(i, xCol)][currentVector].push_back(inputFile(i, j));
	    }
	    else{
	      //Add a blank string to keep indexing similar
	      plotNameParameters[inputFile(i, xCol)][currentVector].push_back("");
	    }
	  }
	  
	  //Update maximum curve count value
	  if(plotData[inputFile(i, xCol)].size() > curveCount){
	    curveCount = plotData[inputFile(i, xCol)].size();
	  }
	}
      }
    }

    //Add all possible blank curves
    for(unsigned i = 0; i < curveCount; i++){
      curveData.push_back(std::vector<std::tuple<double, double>>{});
    }
    
    //For every unique value found
    for(unsigned i = 0; i < columnUniqueValues[possiblePlots[plotId][0]].size(); i++){
      //For each possible curve count saved in the plot data
      for(unsigned j = 0; (j < curveCount) && (j < plotData[columnUniqueValues[possiblePlots[plotId][0]][i]].size()); j++){
	//Copy the found tuple to the appropriate curve
	curveData[j].push_back(plotData[columnUniqueValues[possiblePlots[plotId][0]][i]][j]);
      }
    }

  }
  
  return 0;
}
