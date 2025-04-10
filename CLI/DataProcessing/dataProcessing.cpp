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
#define X_PLOT_COLUMNS "x_columns"
#define Y_PLOT_COLUMNS "y_columns"
#define Z_PLOT_COLUMNS "z_columns"
#define DEBUG "debug"
#define X_RANGE_LIMIT "x_range"
#define Y_RANGE_LIMIT "y_range"
#define Z_RANGE_LIMIT "z_range"
#define PLOT_TITLE "plot_title"
#define SUBTITLE_COLUMNS "subtitle_columns"
#define LEGEND_TITLE "legend_title"
#define LEGEND_COLUMNS "legend_columns"
#define LEGEND_FONT_SIZE "legend_font_size"
#define GNUPLOT_NEWLINE "\\n"

//Parse out the units for the provided string
std::string ParseUnits(std::string parameter){
  std::string units = "";
  int start = -1, stop = -1;

  //For every character in the string or until start and stop values are set
  for(unsigned i = (parameter.length() - 1); ((stop < 0) || (start < 0)) && (i > 0); i--){
    //If current character is a closing parenthesis
    if(parameter[i] == ')'){
      stop = i;
    }
    //If current character is an opening parenthesis
    else if(parameter[i] == '('){
      start = i;
    }
  }
  
  //If stop is after start and both are not equal
  if((stop > start) && (start != stop)){
    //Slice off the discovered unit portion
    units = " " + parameter.substr(start + 1, stop - start - 1);
  }

  //Return string
  return units;
}

//Convert title string to remove any special characters
std::string GenerateFileName(std::string title){

  for(unsigned i = 0; i < title.length(); i++){
    // If character matches any typical special character
    if(title[i] == '(' || title[i] == ')' || title[i] == ' ' || title[i] == '.' || title[i] == '\\' || title[i] == '/'){
      // Replace with underscore
      title[i] = '_';
    }
  }
  
  return title;
}

int main(int argc, char *argv[]){

  //Instantiate abort flag
  bool abort = false;

  //Instantiate value for how many fields are in a row of the subtitle line
  unsigned subtitleLineColumns = 5;
  //Instantiate command line input parser
  Utilities::CLIParser cli;

  //Calculate flag types to be accepted
  int flagTypeOne = Utilities::CLIParser::Dash | Utilities::CLIParser::Space;
  int flagTypeTwo = Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Space;
  int flagTypeThree = Utilities::CLIParser::Dash | Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Standalone;

  //Add desired flags to parser
  cli.Add(Utilities::CLIParser::DESCRIPTION, std::vector<std::string>{""}, std::vector<int>{flagTypeThree}, "A program which can process CSV files into plots using GnuPlot.");
  cli.Add(Utilities::CLIParser::HELP, std::vector<std::string>{"h", "help"}, std::vector<int>{flagTypeThree}, "Display this help message.");
  cli.Add(Utilities::CLIParser::VERBOSE, std::vector<std::string>{"v", "verbose"}, std::vector<int>{flagTypeThree}, "Trigger verbose program output.");
  cli.Add(DEBUG, std::vector<std::string>{"d", "debug"}, std::vector<int>{flagTypeThree}, "Trigger debug program output.");
  cli.Add(INPUT_FILE, std::vector<std::string>{"i", "input"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The input CSV file to process.");
  cli.Add(OUTPUT_DIRECTORY, std::vector<std::string>{"o", "output"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The output directory where to place the processed files.");
  cli.Add(X_PLOT_COLUMNS, std::vector<std::string>{"xc", "x-columns"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "A comma-separated list of the x-columns to plot.");
  cli.Add(Y_PLOT_COLUMNS, std::vector<std::string>{"yc", "y-columns"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "A comma-separated list of the y-columns to plot.");
  cli.Add(Z_PLOT_COLUMNS, std::vector<std::string>{"zc", "z-columns"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "A comma-separated list of the z-columns to plot.");
  cli.Add(X_RANGE_LIMIT, std::vector<std::string>{"xr", "x-range"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "A comma-separated list of value to use for X-axis limits. Applies to both two- and three-dimensional plots.");
  cli.Add(Y_RANGE_LIMIT, std::vector<std::string>{"yr", "y-range"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "A comma-separated list of value to use for Y-axis limits. Applies to both two- and three-dimensional plots.");
  cli.Add(Z_RANGE_LIMIT, std::vector<std::string>{"zr", "z-range"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "A comma-separated list of value to use for Z-axis limits. Only applies to three-dimensional plots.");
  cli.Add(PLOT_TITLE, std::vector<std::string>{"t", "title"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The replacement title string for auto-generated title.");
  cli.Add(SUBTITLE_COLUMNS, std::vector<std::string>{"sc", "subtitle-columns"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The number of fields per line of the automatically generated subtitle. Default is " + std::to_string(subtitleLineColumns) + ".");
  cli.Add(LEGEND_TITLE, std::vector<std::string>{"lt", "legend-title"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The string to use instead of the auto-generated legend title.");
  cli.Add(LEGEND_COLUMNS, std::vector<std::string>{"lc", "legend-columns"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The number of columns to display in the legend instead of auto-generated quantity.");
  cli.Add(LEGEND_FONT_SIZE, std::vector<std::string>{"lfs", "legend-font-size"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The font size value to use in the legend instead of auto-generated quantity.");
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //Print a message and exit if the user called for the help routine
  if(cli.Help()){
    return 0;
  }

  //Determine output verbosity
  bool debug = cli.Present(DEBUG);
  bool verbose = debug || cli.Present(Utilities::CLIParser::VERBOSE);
  
  //Declare default output as current directory
  std::string input, output = "."; //Might break on Windows? Use Filesystem library?
  std::vector<std::string> xColumnWhitelist, yColumnWhitelist, zColumnWhitelist;

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
  if(cli.Present(X_PLOT_COLUMNS)){
    //Update the whitelist
    xColumnWhitelist = cli.GetList(X_PLOT_COLUMNS);
  }

  //If a list was provided
  if(cli.Present(Y_PLOT_COLUMNS)){
    //Update the whitelist
    yColumnWhitelist = cli.GetList(Y_PLOT_COLUMNS);
  }

  //If a list was provided
  if(cli.Present(Z_PLOT_COLUMNS)){
    //Update the whitelist
    zColumnWhitelist = cli.GetList(Z_PLOT_COLUMNS);
  }

  //If a column count was provided
  if(cli.Present(SUBTITLE_COLUMNS)){
    subtitleLineColumns = cli.GetNumeric(SUBTITLE_COLUMNS);
  }
  
  //Open input file
  CommaSeparatedValues inputFile;
  inputFile.Open(input);
  inputFile.Read();

  //Get input file size
  Utilities::Limits inputSize = inputFile.Size();
    
  //Store a mapping of column header to index
  std::map <std::string, unsigned> columnToIndexMapping;
  std::vector<std::string> columnUnitsMapping;
  for(unsigned i = 0; i < inputSize.Columns; i++){
    columnToIndexMapping[inputFile(0, i)] = i;

    //Parse out units and add to list
    columnUnitsMapping.push_back(ParseUnits(inputFile(0, i)));

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
  if(debug){
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
  std::vector<std::string> constantParameters;
  for(auto const& it : columnUniqueValues){
    if(columnUniqueValues[it.first].size() == 1){
      //Append the column header and unique value in a comma-separated list
      constantParameters.push_back(it.first + ": " + columnUniqueValues[it.first][0]);
    }
  }

  //Print out header line without consideration of subtitle count
  if(debug){
    std::cout << "Plot Subtitle" << std::endl;
    if(constantParameters.size() > 0){
      std::cout << constantParameters[0];
      for(unsigned i = 0; i < constantParameters.size(); i++){
	std::cout << ", " << constantParameters[i];
      }
    }
    std::cout << std::endl;
  }

  std::vector<std::vector<std::string>> possiblePlots;
  std::map<std::string, std::vector<std::tuple<double, double, double>>> plotData;
  std::map<std::string, std::vector<std::vector<std::string>>> plotNameParameters;
  std::vector<std::vector<std::tuple<double, double, double>>> curveData;
  Utilities::ConvertedData xResult, yResult, zResult;
  std::string graphTitle;
  unsigned curveCount = 0;
  std::vector<bool> plotInclude = {false, false, false};
  bool xWhiteListEmpty = (xColumnWhitelist.size() == 0), yWhiteListEmpty = (yColumnWhitelist.size() == 0), zWhiteListEmpty = (zColumnWhitelist.size() == 0);
  
  //For every column in the input file
  for(auto const& xColumn : columnUniqueValues){
    //Calculate X-column flag
    plotInclude[0] = xWhiteListEmpty || Utilities::ContainsItem<std::string>(xColumnWhitelist, xColumn.first);

    //If column has more than one value
    if(plotInclude[0] && (columnUniqueValues[xColumn.first].size() > 1)){
      
      //Attempt to pair with every other possible column
      for(auto const& yColumn : columnUniqueValues){
	//Calcualte Y-column flag
	plotInclude[1] = yWhiteListEmpty || Utilities::ContainsItem<std::string>(yColumnWhitelist, yColumn.first);

	//So long as there is more than one value in the column and it is not the X-Column
	if(plotInclude[1] && (columnUniqueValues[yColumn.first].size() > 1) && (xColumn.first != yColumn.first)){

	  //Add a two dimensional plot to the list
	  possiblePlots.push_back(std::vector<std::string> { xColumn.first, yColumn.first });

	  //Attempt to pair X- and Y-columns with every other possible column
	  for(auto const& zColumn : columnUniqueValues){
	    //Reset Z-column flag
	    plotInclude[2] = zWhiteListEmpty || Utilities::ContainsItem<std::string>(zColumnWhitelist, zColumn.first);

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
  for(unsigned plotId = 0, xCol=0, yCol=0, zCol=0; plotId < possiblePlots.size(); plotId++){
    
    //Build graph title string
    graphTitle = "";
    for(unsigned j = possiblePlots[plotId].size() - 1; j > 0; j--){
      graphTitle += possiblePlots[plotId][j] + " vs. ";
    }
    graphTitle += possiblePlots[plotId][0];
    
    //If verbose operation was requested
    if(verbose){
      //Print out columns being graphed
      std::cout << "Generating Graph: " << graphTitle << " ";

      //If printing a 3D graph
      if(possiblePlots[plotId].size() > 2){
	//Print newline
	std::cout << std::endl;
      }
    }
            
    //Clear all previous values
    plotData.clear();
    curveData.clear();
    plotNameParameters.clear();
    curveCount = 0;
          
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

    //For every data row in the output file
    for(unsigned i = 1; i < inputSize.Rows; i++){
      xResult = Utilities::ConvertValue_Double(inputFile(i, xCol));
      yResult = Utilities::ConvertValue_Double(inputFile(i, yCol));
      zResult = Utilities::ConvertValue_Double(inputFile(i, zCol));
      
      //If errors did not result
      if(!xResult.error && !yResult.error && !zResult.error){
	//If more than two dimensions
	if(zCol != yCol){
	  // Assume only one curve for the heatmap
	  plotData[possiblePlots[plotId][2]].emplace_back(xResult.value, yResult.value, zResult.value);
	}
	else{
	  //Add data to appropriate list
	  plotData[inputFile(i, xCol)].emplace_back(xResult.value, yResult.value, zResult.value);
	  
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
	plotData[value.first].emplace_back(xResult.value, nan, nan);
      }
    }

    //Declare GNUPlot object
    Gnuplot gp;
    //Instantiate string to hold one line plot string
    std::string gpPlotLine = "", titleOffset = "";
    std::vector<std::string> axisLimits;
    
    gp << "set output \"" << output << "/" << GenerateFileName(graphTitle) << ".png\"" << std::endl;
    gp << "set terminal png size 1920,1080 font \" ,30\"" << std::endl;
    gp << "set ylabel \"" << possiblePlots[plotId][1] << "\"" << std::endl;
    gp << "set xlabel \"" << possiblePlots[plotId][0] << "\"" << std::endl;
    
    //If plot title was provided
    if(cli.Present(PLOT_TITLE)){
      //Overwrite the graph title string
      gp << "set title \"" << cli.Get(PLOT_TITLE);
    }
    else{
      //Use auto-generated title
      gp << "set title \"" << graphTitle;
    }

    //Write subtitle line
    for(unsigned subtitleParam = 0; subtitleParam < constantParameters.size(); ){
      //Write a newline
      gp << GNUPLOT_NEWLINE << "{/*0.5 ";
      for(unsigned subtitleParamCur = 0; (subtitleParam < constantParameters.size()) && (subtitleParamCur < subtitleLineColumns); subtitleParamCur++, subtitleParam++){
	//Write the parameter string built earlier
	gp << constantParameters[subtitleParam];

	//If not the final column
	if((subtitleParam + 1) < constantParameters.size()){
	  //Write a comma
	  gp << ", ";
	}
      }
      gp << "}";
    }
    
    gp << "\"" << std::endl;
    
    if(cli.Present(X_RANGE_LIMIT)){
      axisLimits = cli.GetList(X_RANGE_LIMIT);
      gp << "set xrange [" << axisLimits[0] << ":" << axisLimits[1] << "]" << std::endl;
    }

    if(cli.Present(Y_RANGE_LIMIT)){
      axisLimits = cli.GetList(Y_RANGE_LIMIT);
      gp << "set yrange [" << axisLimits[0] << ":" << axisLimits[1] << "]" << std::endl;
    }

    //If more than two dimensions
    if(zCol != yCol){
      double minXVal = std::numeric_limits<double>::max(), maxXVal = std::numeric_limits<double>::min(), minYVal = std::numeric_limits<double>::max(), maxYVal = std::numeric_limits<double>::min(), curVal;

      //Calculate the 3D plot top margin offset value from the number of parameters and desired columns to match 2D plots
      unsigned titleMargin = (constantParameters.size() / subtitleLineColumns) + 1;

      //If present
      if(cli.Present(Z_RANGE_LIMIT)){
	//Set the Z-range limit
	axisLimits = cli.GetList(Z_RANGE_LIMIT);
	gp << "set cbrange [" << axisLimits[0] << ":" << axisLimits[1] << "]" << std::endl;
      }
      
      // Determine the minimum and maximum values for the X- and Y-axis.
      for(unsigned i = 0; i < plotData[possiblePlots[plotId][2]].size(); i++){
	// Update minimum and maximum if the value exceeds the stored
	curVal = std::get<0>(plotData[possiblePlots[plotId][2]][i]);
	if(curVal > maxXVal){
	  maxXVal = curVal;
	}
	
	if(curVal < minXVal){
	  minXVal = curVal;
	}

	// Update minimum and maximum if the value exceeds the stored
	curVal = std::get<1>(plotData[possiblePlots[plotId][2]][i]);
	if(curVal > maxYVal){
	  maxYVal = curVal;
	}

	if(curVal < minYVal){
	  minYVal = curVal;
	}

      }

      //Update the 3D plot top margin and title offset values to allow for dynamic alteraton of the subtitle
      gp << "set tmargin " << std::to_string(titleMargin) << std::endl;
      gp << "set title offset 0," << std::to_string(titleMargin) << std::endl;
      
      // Generate a MATLAB color scheme macro
      gp << "set macros" << std::endl << "MATLAB = \"defined (0  0.0 0.0 0.5, 1  0.0 0.0 1.0, 2  0.0 0.5 1.0, 3  0.0 1.0 1.0, 4  0.5 1.0 0.5, 5  1.0 1.0 0.0, 6  1.0 0.5 0.0, 7  1.0 0.0 0.0, 8  0.5 0.0 0.0)\"" << std::endl;
      gp << "set style data linespoints" << std::endl;
      gp << "set view map" << std::endl;
      gp << "set hidden3d" << std::endl;
      gp << "set dgrid3d 50,50 qnorm 2" << std::endl;
      gp << "set pm3d interpolate 4,4" << std::endl;
      gp << "set palette @MATLAB" << std::endl;
      gp << "set cblabel \"" << possiblePlots[plotId][2] << "\"" << std::endl;
      gp << "set xrange [" << std::to_string(minXVal) << ":" << std::to_string(maxXVal)  << "]" << std::endl;
      gp << "set yrange [" << std::to_string(minYVal) << ":" << std::to_string(maxYVal)  << "]" << std::endl;

      gp << "splot ";
      // If debugging
      if(debug){
	// Create a named temporary file in the current directory
	gp << gp.file1d(plotData[possiblePlots[plotId][2]], GenerateFileName(graphTitle));
      }
      else{
	// Allow utility to create temporary file which is cleaned up later
	gp << gp.file1d(plotData[possiblePlots[plotId][2]]);
      }
      gp << " notitle with pm3d" << std::endl;
    }
    else{
      //Declare variable to hold previous parameter value for comparison
      std::string curveLabel;
      //Declare variable to hold all X values
      std::vector<std::string> plotXValues, curveLabels;
      //Declare variable to hold indication if the parameter string is stable for a curve
      std::vector<bool> parametersTable;
      bool keyTitleGenerated = true;
      
      //Get all keys of the mapping 
      for(auto const& xValue : plotNameParameters){
	plotXValues.push_back(xValue.first);
      }
      
      for(unsigned j = 0; j < inputSize.Columns; j++){
	//Initialize parameter table flags
	parametersTable.push_back(true);
      }

      std::vector<std::string> xValList;
      for(auto const& it : plotNameParameters){
	xValList.push_back(it.first);
      }
      
      //Add all possible blank curves
      for(unsigned i = 0; i < curveCount; i++){
	curveData.push_back(std::vector<std::tuple<double, double, double>>{});

	//Reset the parameter flags
	for(unsigned j = 0; j < parametersTable.size(); j++){
	  parametersTable[j] = true;
	}

	//Reduce plot name parameters to only unchanging values for every curve
	for(unsigned paramIndex = 0; paramIndex < inputSize.Columns; paramIndex++){

	  //Determine which parameter columns are unchanging
	  for(unsigned j = 1; j < plotXValues.size(); j++){	    
	    //Calculate if current parameter is equivalent to first parameter and not blank
	    parametersTable[paramIndex] = parametersTable[paramIndex] && (i < plotNameParameters[plotXValues[j]].size()) && (i < plotNameParameters[plotXValues[0]].size());
	    parametersTable[paramIndex] = parametersTable[paramIndex] && (plotNameParameters[plotXValues[0]][i][paramIndex] == plotNameParameters[plotXValues[j]][i][paramIndex]);	    
	    parametersTable[paramIndex] = parametersTable[paramIndex] && (plotNameParameters[plotXValues[j]][i][paramIndex] != "");
	  }
	}

	//Build the curve string and save it for later while also building key label string
	curveLabel = "";
	for(unsigned j = 0; j < inputSize.Columns; j++){
	  //If first stable parameter
	  if(parametersTable[j] && (curveLabel == "")){
	    //Start curve label
	    curveLabel = plotNameParameters[plotXValues[0]][i][j] + columnUnitsMapping[j];
	  }
	  //If not first stable parameter
	  else if(parametersTable[j]){
	    //Append curve label
	    curveLabel += ", " + plotNameParameters[plotXValues[0]][i][j] + columnUnitsMapping[j];
	  }
	}

	//If curve label is still blank
	if(curveLabel == ""){
	  //Create generic curve label name
	  curveLabel = "Curve " + std::to_string(i);
	  keyTitleGenerated = false;
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

      if(debug){
	std::cout << "with " << curveLabels.size() << " curves." << std::endl;
      }
      else if(verbose){
	std::cout << std::endl;
      }

      //Send GNU Plot parameters for a 2D plot
      int keyColumns = curveCount / 32;
      //If user provided a columns value
      if(cli.Present(LEGEND_COLUMNS)){
	//Replace auto-generated value
	keyColumns = cli.GetNumeric(LEGEND_COLUMNS);
      }
      
      int fontSize = 20 / keyColumns;
      //If user provided a font size
      if(cli.Present(LEGEND_FONT_SIZE)){
	//Replace auto-generated value
	fontSize = cli.GetNumeric(LEGEND_FONT_SIZE);
      }
      
      gp << "unset warnings" << std::endl;
      gp << "set datafile missing 'nan'" << std::endl;
      gp << "set key reverse Left outside" << std::endl;
      gp << "set grid" << std::endl;
      gp << "set style data linespoints" << std::endl;
      gp << "set key title \"";

      //If the user provided a title
      if(cli.Present(LEGEND_TITLE)){
	//Replace the auto-generated label
        gp << cli.Get(LEGEND_TITLE);
      }//If every curve has a proper label
      else if(keyTitleGenerated){
	//Use the last parameter list to auto-generate title
	for(unsigned j = 0, first = 0; j < inputSize.Columns; j++){
	  //If first stable parameter
	  if(parametersTable[j] && (first == 0)){
	    //Start key label
	    gp << inputFile(0,j);
	    first++;
	  }
	  //If not first stable parameter
	  else if(parametersTable[j]){
	    //Append key label with next column header and a newline character
	    gp << ", ";
	    //If only one column
	    if(keyColumns == 1){
	      // Keep key small by inserting a newline
	      gp << GNUPLOT_NEWLINE;
	    }
	    gp << inputFile(0,j);
	  }
	}
      }//Otherwise
      else{
	//Use a generic key title
	gp << "Legend";
      }
      
      gp << "\" font \"," << fontSize << " \"" << std::endl;
      gp << "set key font \"," << fontSize << "\"" << std::endl;
      gp << "set key columns " << keyColumns << std::endl;
      
      // If only one curve
      if(curveCount == 1){
	// Remove legend to ignore any discovered label
	gp << "set key off" << std::endl;
      }
      
      gp << "plot ";

      //Build the plot string one-liner
      for(unsigned i = 0; i < curveData.size(); i++){
	if(curveData[i].size() > 0){
	  // If debugging
	  if(debug){
	    // Use a named temporary file for graphing
	    gp << gp.file1d(curveData[i], GenerateFileName(graphTitle) + "_" + std::to_string(i));
	  }
	  else{
	    //Utilize temporary file for graphing
	    gp << gp.file1d(curveData[i]);
	  }
	  
	  gp << " with linespoints title '" + curveLabels[i] + "'";
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
