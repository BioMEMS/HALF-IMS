//Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <limits>
#include <vector>
#include <cstdio>

//HALF-IMS libraries
#include "Filter.h"
#include "CLIParser.h"
#include "CommaSeparatedValues.h"
#include "Checker.h"
#include "gnuplot-iostream.h"
#include "Utilities.h"

//Pre-processor Variables
#define OUTPUT_DIRECTORY "output"
#define INPUT_FILE "input"
#define PLOT_COLUMNS "columns"

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
  cli.Add(PLOT_COLUMNS, std::vector<std::string>{"c", "columns"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "A comma-separated list of columns to plot.");
  
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
  std::vector<std::string> columnWhitelist;
  
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

  //If a list was provided
  if(cli.Present(PLOT_COLUMNS)){
    //Update the whitelist
    columnWhitelist = cli.GetList(PLOT_COLUMNS);
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
  std::map<std::string, std::vector<std::pair<double, double>>> plotData;
  std::map<std::string, std::vector<std::vector<std::string>>> plotNameParameters;
  std::vector<std::vector<std::pair<double, double>>> curveData;
  Utilities::ConvertedData xResult, yResult, zResult;
  std::string graphTitle;
  unsigned curveCount = 0;
  std::vector<bool> plotInclude = {false, false, false};
  bool whiteListEmpty = (columnWhitelist.size() == 0);
  
  //For every column in the input file
  for(auto const& xColumn : columnUniqueValues){
    //Calculate X-column flag
    plotInclude[0] = whiteListEmpty || Utilities::ContainsItem<std::string>(columnWhitelist, xColumn.first);

    //If column has more than one value
    if(plotInclude[0] && (columnUniqueValues[xColumn.first].size() > 1)){
      
      //Attempt to pair with every other possible column
      for(auto const& yColumn : columnUniqueValues){
	//Calcualte Y-column flag
	plotInclude[1] = whiteListEmpty || Utilities::ContainsItem<std::string>(columnWhitelist, yColumn.first);

	//So long as there is more than one value in the column and it is not the X-Column
	if(plotInclude[1] && (columnUniqueValues[yColumn.first].size() > 1) && (xColumn.first != yColumn.first)){

	  //Add a two dimensional plot to the list
	  possiblePlots.push_back(std::vector<std::string> { xColumn.first, yColumn.first });

	  //Attempt to pair X- and Y-columns with every other possible column
	  for(auto const& zColumn : columnUniqueValues){
	    //Reset Z-column flag
	    plotInclude[2] = whiteListEmpty || Utilities::ContainsItem<std::string>(columnWhitelist, zColumn.first);

	    //So long as there is more than one value in the column and it is not the X- or Y-Columns
	    if(plotInclude[2] && (columnUniqueValues[zColumn.first].size() > 1) && (xColumn.first != zColumn.first) && (yColumn.first != zColumn.first)){
	      
	      //Add a three dimensional plot to the list
	      possiblePlots.push_back(std::vector<std::string> {xColumn.first, yColumn.first, zColumn.first});
	    }
	  }
	}
      }
    }
  }

  //For every possible plot
  for(unsigned plotId = 0, xCol=0, yCol=0, zCol=0; plotId < possiblePlots.size(); plotId++, curveCount = 0){
    //Build graph title string
    graphTitle = "";
    for(unsigned j = possiblePlots[plotId].size() - 1; j > 0; j--){
      graphTitle += possiblePlots[plotId][j] + " vs. ";
    }
    graphTitle += possiblePlots[plotId][0];
    
    //If verbose operation was requested
    if(verbose){
      //Print out columns being graphed
      std::cout << std::endl << "Generating Graph: " << graphTitle << " ";
    }
    
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
	//If more than two dimensions
	if(zCol != yCol){
	  //
	}
	else{
	  //Add data to appropriate list
	  plotData[inputFile(i, xCol)].emplace_back(xResult.value, yResult.value);
	  
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

    // For every X-value in the plot data
    double nan = std::numeric_limits<double>::quiet_NaN();
    for(auto const& value : plotData){
      // Convert X-value to a numeric 
      xResult = Utilities::ConvertValue_Double(value.first);
      // Add blank values to ensure that each curve has values at that point
      for(; plotData[value.first].size() < curveCount;){
	plotData[value.first].emplace_back(xResult.value, nan);
      }
    }
    
    //If more than two dimensions
    if(zCol != yCol){

      //Send newline to format verbose mode correctly
      if(verbose){
	std::cout << std::endl;
      }
    }
    else{
      //Declare variable to hold previous parameter value for comparison
      std::string curveLabel;
      //Declare variable to hold all X values
      std::vector<std::string> plotXValues, curveLabels;
      //Declare variable to hold indication if the parameter string is stable for a curve
      std::vector<bool> parametersTable;
      
      //Get all keys of the mapping 
      for(auto const& xValue : plotNameParameters){
	plotXValues.push_back(xValue.first);
      }

      for(unsigned j = 0; j < inputSize.Columns; j++){
	parametersTable.push_back(true);
      }
      
      //Add all possible blank curves
      for(unsigned i = 0; i < curveCount; i++){
	curveData.push_back(std::vector<std::pair<double, double>>{});

	//Reset the parameter flags
	for(unsigned j = 0; j < parametersTable.size(); j++){
	  parametersTable[j] = true;
	}
	
	//Reduce plot name parameters to only unchanging values for every curve
	for(unsigned paramIndex = 0; paramIndex < inputSize.Columns; paramIndex++){

	  //Determine which parameter columns are unchanging
	  for(unsigned j = 0; j < plotXValues.size(); j++){
	    //Calculate if current parameter is equivalent to first parameter and not blank
	    parametersTable[paramIndex] = (i < plotNameParameters[plotXValues[j]].size()) && parametersTable[paramIndex] && (plotNameParameters[plotXValues[j]][0][paramIndex] == plotNameParameters[plotXValues[j]][i][paramIndex]) && (plotNameParameters[plotXValues[j]][i][paramIndex] != "");
	  }
	}

	//Build the curve label string and save it for later
	curveLabel = "";
	for(unsigned j = 0; j < inputSize.Columns; j++){
	  //If first stable parameter
	  if(parametersTable[j] && (curveLabel == "")){
	    //Start curve label
	    curveLabel = inputFile(0,j) + " " + plotNameParameters[plotXValues[0]][0][j];
	  }
	  //If not first stable parameter
	  else if(parametersTable[j]){
	    //Append curve label
	    curveLabel += ", " + plotNameParameters[plotXValues[0]][0][j];
	  }
	}

	//If curve label is still blank
	if(curveLabel == ""){
	  //Create generic curve label name
	  curveLabel = "Curve " + std::to_string(i);
	}

	//Save generated curve label
	curveLabels.push_back(curveLabel);
	
      }
      
      //For every unique value found
      for(unsigned i = 0; i < columnUniqueValues[possiblePlots[plotId][0]].size(); i++){
	//For each possible curve count saved in the plot data
	for(unsigned j = 0; (j < curveCount) && (j < plotData[columnUniqueValues[possiblePlots[plotId][0]][i]].size()); j++){
	  //Copy the found tuple to the appropriate curve
	  curveData[j].push_back(plotData[columnUniqueValues[possiblePlots[plotId][0]][i]][j]);
	}
      }

      if(verbose){
	std::cout << " with " << curveLabels.size() << " curves." << std::endl;
      }
      
      //Declare GNUPlot object
      Gnuplot gp;
      //Instantiate string to hold one line plot string
      std::string gpPlotLine = "";
      
      //Send GNU Plot parameters for a 2D plot
      gp << "set macros" << std::endl << "MATLAB = \"(0  0.0 0.0 0.5, 1  0.0 0.0 1.0, 2  0.0 0.5 1.0, 3  0.0 1.0 1.0, 4  0.5 1.0 0.5, 5  1.0 1.0 0.0, 6  1.0 0.5 0.0, 7  1.0 0.0 0.0, 8  0.5 0.0 0.0)\"" << std::endl;
      gp << "unset warnings" << std::endl;
      gp << "set datafile missing 'nan'" << std::endl;
      gp << "set terminal png size 1920,1080 font \" ,30\"" << std::endl;
      gp << "set output \"" << output << "/" << graphTitle << ".png\"" << std::endl;
      gp << "set key reverse Left outside" << std::endl;
      gp << "set grid" << std::endl;
      gp << "set style data linespoints" << std::endl;
      gp << "set key title \"Legend\" font \",20\"" << std::endl;
      gp << "set key font \",20\"" << std::endl;
      gp << "set key columns 1" << std::endl;
      gp << "set title \"" << graphTitle << "\\n{/*0.5 " << constantParameters << "}\"" << std::endl;
      gp << "set ylabel \"" << possiblePlots[plotId][1] << "\"" << std::endl;
      gp << "set xlabel \"" << possiblePlots[plotId][0] << "\"" << std::endl;
      gp << "plot ";

      // If only one curve
      if(curveCount == 1){
	// Ignore any discovered label
	curveLabels[0] = "Data";
      }
      
      //Build the plot string one-liner
      for(unsigned i = 0; i < curveData.size(); i++){
	if(curveData[i].size() > 0){
	  //Utilize temporary file for graphing
	  gp << gp.file1d(curveData[i]) << " with linespoints title '" + curveLabels[i] + "'";
	  if(i < (curveData.size() - 1)){
	    gp <<", ";
	  }
	  else{
	    gp << std::endl;
	  }
	}
      }
      
      //Send GNU Plot line
      //gp << gpPlotLine.substr(0, gpPlotLine.length()-2) << std::endl;
      //Send GNU plot data
      //gp.send1d(gpPlotData);
      
    }
    
  }
  return 0;
}
