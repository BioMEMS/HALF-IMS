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
#define SAMPLE_COMPRESSION_COUNT "sample_compression"
#define SAMPLE_PER_SEGMENT "samples_per_segment"
#define CHEMICAL_COLUMN_INDEX "chemical_index"
#define DOPANT_COLUMN_INDEX "dopant_index"
#define RELATIVE_CALCULATION "rel_calc"
#define BASELINE_REMOVAL "baseline_removal"
#define SYSTEM_PRESSURE "pressure"
#define SYSTEM_TEMPERATURE "temperature"
#define SYSTEM_GAP_SIZE "gap_size"
#define SAVE_ALL_COLUMNS "no_delete_columns"
#define NO_DOPANT_COLUMN_PRESENT "no_dopant_present"
#define LABVIEW_DATA_COLUMNS 7
#define LABVIEW_TIME_COLUMN_INDEX 0
#define CSV_SHUTTER_COLUMN_INDEX 11
#define CSV_DET_ONE_COLUMN_INDEX 5
#define CSV_DET_TWO_COLUMN_INDEX 6
#define ALLOWED_DIGITS 3
#define ALLOWED_DIGITS_CONTROL_RATIO 6

//Determine if the column should be deleted based upon the header
bool MarkColumnDeleted(std::string column){
  bool status = false;

  //Check for columns matching '{-|+}V_{l|s} (V)'
  status |= (column.find("V_") != std::string::npos) && (column.find("(V)") != std::string::npos);

  //Check for detector voltage columns
  status |= (column.find("Detector") != std::string::npos) && (column.find("(V)") != std::string::npos);
  
  //Check for MIPS Set/Read columns
  status |= (column.find("MIPS") != std::string::npos) && (column.find("Ch.") != std::string::npos);

  //Check for Syringe Volume/Pump columns
  status |= (column.find("Syringe") != std::string::npos) && (column.find("mL") != std::string::npos);

  //Check for Syringe Volume/Pump columns
  status |= (column.find("MFC Setting") != std::string::npos);

  
  return status;
}

//Split the provided CSV line into numeric values
std::vector<std::string> SplitLine(std::string line){
  std::vector<std::string> results;
  std::string temp;
  
  //For each character in the line
  for(unsigned i = 0, start = 0, stop = 0; i < line.length(); i++){
    //If the character is a comma or at the end of the string
    if((line[i] == ',') || ((i + 1) == line.length())){
      //Update the stop index
      stop = i;
    }

    //If the stop and start are not equal
    if(stop != start){
      //Take substring      
      temp = line.substr(start, stop-start);

      //Add substring to list
      results.push_back(temp);
      
      //Update indices to be one character beyond current
      stop += 1;
      start = stop;
    }
  }
  ;
  return results;
}

//Convert provided vect
std::vector<double> ConvertLine(unsigned lineNumber, std::vector<std::string> line, std::vector<unsigned> chemicalIndices){
  std::vector<double> results;
  Utilities::ConvertedData result;
  
  //For each element in the line
  for(unsigned i = 0; i < line.size(); i++){
    //Attempt to convert element to a double
    result = Utilities::ConvertValue_Double(line[i]);

    //If there was an error with converting the value
    if(!result.error){
      results.push_back(result.value);
    }
    else{
      //If the current index is not a known chemical index
      if(!Utilities::ContainsItem(chemicalIndices,i)){
	//Print message to error stream
	std::cerr << "Exception in '" << result.msg << "' thrown attempting to convert '" << line[i] << "' at position " << i << " for line " << lineNumber << ". ";
	std::cerr << "A minimum data value has been added as a placeholder to preserve any data spacing." << std::endl;
      }
      
      //Push minimum double value
      results.push_back(std::numeric_limits<double>::min());
    }
  }

  return results;
}

int main(int argc, char *argv[]){

  //Declare default output as current directory
  std::string output, input;
  bool inputFlag, outputFlag, verbose, saveAllColumns;
  unsigned sampleCompressionMaximum = 4, chemicalIndex = 23, dopantIndex = 28;
  double systemTemperature = 30, systemPressure = 26, systemGap = 100E-6, systemGapScaled = 100;
  std::vector<unsigned> chemicalIndices;
    
  //Instantiate command line input parser
  Utilities::CLIParser cli;

  //Calculate flag types to be accepted
  int flagTypeOne = Utilities::CLIParser::Dash | Utilities::CLIParser::Space;
  int flagTypeTwo = Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Space;
  int flagTypeThree = Utilities::CLIParser::Dash | Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Standalone;

  //Add desired flags to parser
  cli.Add(Utilities::CLIParser::DESCRIPTION, std::vector<std::string>{""}, std::vector<int>{flagTypeThree}, "A program to process LabView output CSV file into something that can be processed by GNU Plot for HALF-IMS development. When not provided proper inputs, the program performs no action and exits.");
  cli.Add(Utilities::CLIParser::HELP, std::vector<std::string>{"h", "help"}, std::vector<int>{flagTypeThree}, "Display this help message.");
  cli.Add(Utilities::CLIParser::VERBOSE, std::vector<std::string>{"v", "verbose"}, std::vector<int>{flagTypeThree}, "Trigger verbose program output.");

  //Add user flags
  cli.Add(INPUT_FILE, std::vector<std::string>{"i", "input"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The comma-separated list of input files to process. Final line is assumed to be a blank newline character.");
  cli.Add(OUTPUT_FILE, std::vector<std::string>{"o", "output"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The output directory where to place the processed files.");
  cli.Add(SAMPLE_COMPRESSION_COUNT, std::vector<std::string>{"s", "sections"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The number of LabView sample sections to average together. Default is 4.");
  cli.Add(CHEMICAL_COLUMN_INDEX, std::vector<std::string>{"c", "chemical-column"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The column index which has the chemical name. Default is " + std::to_string(chemicalIndex) + ".");
  cli.Add(DOPANT_COLUMN_INDEX, std::vector<std::string>{"d", "dopant-column"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The column index which has the dopant name. Default is " + std::to_string(dopantIndex) + ".");
  cli.Add(NO_DOPANT_COLUMN_PRESENT, std::vector<std::string>{"ndc", "no-dopant-column"}, std::vector<int>{flagTypeThree, flagTypeThree}, "Indicates if the data does not have a dopant column.");
  cli.Add(RELATIVE_CALCULATION, std::vector<std::string>{"nrc", "no-relative-calculation"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "Stop the execution of the relative calculation step.");
  cli.Add(SAVE_ALL_COLUMNS, std::vector<std::string>{"sac", "save-all-columns"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "Prevent the removal of columns no longer necessary due to calculations.");
  cli.Add(BASELINE_REMOVAL, std::vector<std::string>{"b", "baseline"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "A CSV file which will be used to remove the baseline from the data. Must have equivalent columns to target data file.");
cli.Add(SYSTEM_TEMPERATURE, std::vector<std::string>{"t", "temperature"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The carrier gas temperature, in Celsius, to assume for the data file. Default is " + std::to_string(systemTemperature));
 cli.Add(SYSTEM_PRESSURE, std::vector<std::string>{"p", "pressure"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The chip pressure, in PSI, to assume for the data file. Default is " +std::to_string(systemPressure) + ".");cli.Add(SYSTEM_GAP_SIZE, std::vector<std::string>{"g", "gap-size"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The chip gap size, in meters, to assume for the data file. Default is " +std::to_string(systemGap) + ".");
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //Print a message and exit if the user called for the help routine
  if(cli.Help()){
    return 0;
  }
  
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
      std::cout << "Input File: " << input << std::endl;
      std::cout << "Output File: " << output << std::endl;

      //If baseline removal was provided, report file
      if(cli.Present(BASELINE_REMOVAL)){
	std::cout << "Baseline File: " << cli.Get(BASELINE_REMOVAL) << std::endl;
      }
    }
    
    //If compression count maximum was provided
    if(cli.Present(SAMPLE_COMPRESSION_COUNT)){
      //Update value
      sampleCompressionMaximum = cli.GetNumeric(SAMPLE_COMPRESSION_COUNT);

      //If user passed in a bad value
      if(sampleCompressionMaximum <= 0){
	//Reset to one
	sampleCompressionMaximum = 1;
      }
    }

    //If the chemical column index is provided
    if(cli.Present(CHEMICAL_COLUMN_INDEX)){
      //Update value
      chemicalIndex = cli.GetNumeric(CHEMICAL_COLUMN_INDEX);
    }

    //If the chemical column index is provided
    if(cli.Present(DOPANT_COLUMN_INDEX)){
      //Update value
      dopantIndex = cli.GetNumeric(DOPANT_COLUMN_INDEX);
    }

    //Put chemical and dopant column indices into vector
    chemicalIndices.push_back(chemicalIndex);
    chemicalIndices.push_back(dopantIndex);
    
    //Determine if saving all columns
    saveAllColumns = !cli.Present(SAVE_ALL_COLUMNS);

    //If the system temperature value is provided
    if(cli.Present(SYSTEM_TEMPERATURE)){
      //Override default value with provided
      systemTemperature = cli.GetNumeric(SYSTEM_TEMPERATURE);
    }

    //If the system temperature value is provided
    if(cli.Present(SYSTEM_PRESSURE)){
      //Override default value with provided
      systemPressure = cli.GetNumeric(SYSTEM_PRESSURE);
    }

    //If the system temperature value is provided
    if(cli.Present(SYSTEM_GAP_SIZE)){
      //Override default value with provided
      systemGap = cli.GetNumeric(SYSTEM_GAP_SIZE);
      //Calculate system gap in micrometers
      systemGapScaled = systemGap * 1E6;

      //If user provided zero gap size
      if(systemGap == 0.0){
	//Print error and exit
	std::cerr << "Invalid gap size '" << systemGap << "' provided." << std::endl;
	return 1;
      }
    }

    //Declare file streams for input and output
    std::fstream inputFile, outputFile;

    //Open files
    inputFile.open(input, std::fstream::in);
    outputFile.open(output, std::fstream::out | std::fstream::trunc);

    //Determine if either file is open
    inputFlag = inputFile.is_open();
    outputFlag = outputFile.is_open();
    
    //If both file streams were opened
    if(inputFlag && outputFlag){

      //Write a header line detailing the values in each column
      outputFile << "Time (s),+V_s (V),+V_l (V),-V_s (V),-V_l (V),Detector 1 (V),Detector 2 (V),";

      for(int i = 1; i <= 8; i++){
	outputFile << "MIPS Set Ch. " << i << " (V),";
      }

      for(int i = 1; i <= 8; i++){
	outputFile << "MIPS Read Ch. " << i << " (V),";
      }

      outputFile << "Chemical,Chemical Concentration (ppm),Syringe Volume (mL),Syringe Pump (mL/hr),MFC Setting (mL/min),";
      
      //If the dopant column was indicated to be present
      if(!cli.Present(NO_DOPANT_COLUMN_PRESENT)){
	//Add dopant column headers
	outputFile << "Dopant,Dopant Concentration (ppm),";
      }

      //If the relative calculation was allowed
      if(!cli.Present(RELATIVE_CALCULATION)){
	//Write relative correction column values
	outputFile << "Long Electrode Setting (V),Short Electrode Setting (V),Long Electrode Setting (Td.),Short Electrode Setting (Td.),Detector 1 (pA),Detector 2 (pA)";
      }

      outputFile << std::endl;
      
      //Read the file until the LabView header line is found
      unsigned lineCount = 0;
      for(std::string line = "", column = ""; !inputFile.eof() && (column != "X_Value"); std::getline(inputFile, line), column = line.substr(0,7), lineCount++){}

      std::vector<std::string> splitLine;
      std::vector<double> avgLine, numericLine;
            
      //For each line of the input file
      double avgCount = -1;
      double timeSegmentCompressionCount = 0;
      for(std::string line = "", convertedVal="", chemical="", dopant=""; !inputFile.eof(); std::getline(inputFile, line), lineCount++){
	//Split the line into numeric values
	splitLine = SplitLine(line);
	numericLine = ConvertLine(lineCount, splitLine, chemicalIndices);
	
	//Initialize average line with zeroes
	for(unsigned i = avgLine.size(); i < splitLine.size(); i++){
	  avgLine.push_back(0.0);
	}

	//If line has at least more than chemical index elements
	if(splitLine.size() >= (chemicalIndex + 1)){
	  //Grab the chemical element as indicated by the index
	  chemical = splitLine[chemicalIndex];
	}

	//If line has more than dopant index elements
	if(splitLine.size() >= (dopantIndex + 1)){
	  //Grab the dopant element as indicated by the index
	  dopant = splitLine[dopantIndex];
	}

	//Peek next character to trigger EOF calculation, behavior seems to have changed from C++17 to C++23
	inputFile.peek();

	//If line exceeds LAB View column count
	if((splitLine.size() > LABVIEW_DATA_COLUMNS) || inputFile.eof()){
	  //Increase compression count
	  timeSegmentCompressionCount++;
	}
	
	//If the sample compression count has been exceeded or the end of file has been reached
	if((timeSegmentCompressionCount > sampleCompressionMaximum) || (inputFile.eof())){
	  //Re-use line variable
	  line = "";

	  //Build a string from the line
	  for(unsigned i = 0; i < avgLine.size(); i++){
	    //If within LabView data columns
	    if(i == LABVIEW_TIME_COLUMN_INDEX){
	      //Do nothing as this is the time value
	    }
	    else if(i < LABVIEW_DATA_COLUMNS){
	      //Use line-by-line count for averaging
	      avgLine[i] /= avgCount;
	    }
	    //If not within LabView data columns
	    else{
	      //Use time segment compression count for averaging	      
	      avgLine[i] /= (timeSegmentCompressionCount - 1);
	    }
	    
	    //If the current index is the chemical index
	    if(i == chemicalIndex){
	      //Write the chemical directly
	      convertedVal = chemical;
	    }
	    //If the current index is the dopant index
	    else if(i == dopantIndex){
	      //Write the dopant directly
	      convertedVal = dopant;
	    }
	    else{
	      //Convert value to a string and append to output line
	      convertedVal = std::to_string(Utilities::TruncateValue(avgLine[i],ALLOWED_DIGITS));
	    }
	    line = line + convertedVal + ',';

	    //Reset value to zero
	    avgLine[i] = 0.0;
	  }
	  //Replace last comma with newline character
	  line[line.length() - 1] = '\n';

	  //Write line to the file
	  outputFile << line;
	  
	  //Reset average counts
	  avgCount = 0;
	  timeSegmentCompressionCount = 1;      
	}

	//Increment average count after potential write to preserve row found
	avgCount++;
	
	//Add current line to the growing average
	for(unsigned i = 0; i < splitLine.size(); i++){
	  if(i == 0){
	    avgLine[i] = numericLine[i];
	  }
	  else{
	    avgLine[i] += numericLine[i];
	  }
	}
      }
      
      //Close file streams
      inputFile.close();
      outputFile.close();

      //If relative calculation flag is not present
      if(!cli.Present(RELATIVE_CALCULATION)){	
	//Instantiate counters for later output file truncation
	unsigned rowsDeleted = 0, columnsDeleted = 0;
	
	//Open output file as a CSV
	CommaSeparatedValues csvOutput = CommaSeparatedValues(output);
	csvOutput.Read();
	
	//Get size of output for indexing
	Utilities::Limits outputSize = csvOutput.Size();

	csvOutput(0, outputSize.Columns) = "Temperature (C)";
	csvOutput(0, outputSize.Columns+1) = "Pressure (PSI)";
	csvOutput(0, outputSize.Columns+2) = "Gap Size (um)";
	csvOutput(0, outputSize.Columns+3) = "Control Ratio";

	// Declare long and short electrode temporary values
	double longElectrodeValue, shortElectrodeValue;
	
	//For every other row in the output file
	for(unsigned i = outputSize.Rows - 1; i > 1; i-=2){

	  //For every column in the pair of rows
	  for(unsigned j = 0; j < outputSize.Columns; j++){	   
	    //If either of the detector columns
	    if((j == CSV_DET_ONE_COLUMN_INDEX) || (j == CSV_DET_TWO_COLUMN_INDEX)){
	      //Subtract background detector measurement from analyte detector measurement
	      csvOutput(i-1, j) = std::to_string(Utilities::TruncateValue(std::abs(std::stod(csvOutput(i-1,j)) - std::stod(csvOutput(i,j))), ALLOWED_DIGITS));
	    }
	    //If the time column index
	    else if (j == LABVIEW_TIME_COLUMN_INDEX){
	      //Copy value
	      csvOutput(i-1, j) = csvOutput(i,j);
	    }
	    
	    //Delete value from column
	    csvOutput(i, j) = "";	    
	  }

	  // Calculat elong and short values from the settings
	  longElectrodeValue = Utilities::TruncateValue(std::stod(csvOutput(i-1,7)) - std::stod(csvOutput(i-1,9)), ALLOWED_DIGITS);
	  shortElectrodeValue = Utilities::TruncateValue(std::stod(csvOutput(i-1,10)) - std::stod(csvOutput(i-1,8)), ALLOWED_DIGITS);
	  
	  //Store long and short electrode voltage settings 
	  csvOutput(i-1, outputSize.Columns-6) = std::to_string(longElectrodeValue);
	  csvOutput(i-1, outputSize.Columns-5) = std::to_string(shortElectrodeValue);

	  //Convert long and short electrode settings to Townsends
	  csvOutput(i-1, outputSize.Columns-4) = std::to_string(Utilities::TruncateValue(Utilities::ConvertValue_Townsends(systemTemperature,systemPressure,systemGap,longElectrodeValue), ALLOWED_DIGITS));
	  csvOutput(i-1, outputSize.Columns-3) = std::to_string(Utilities::TruncateValue(Utilities::ConvertValue_Townsends(systemTemperature,systemPressure,systemGap,shortElectrodeValue), ALLOWED_DIGITS));

	  //Calculate ideal detector current
	  csvOutput(i-1, outputSize.Columns-2) = std::to_string(Utilities::TruncateValue((1E12)*Utilities::CalculateCurrent(std::stod(csvOutput(i-1,5))), ALLOWED_DIGITS));
	  csvOutput(i-1, outputSize.Columns-1) = std::to_string(Utilities::TruncateValue((1E12)*Utilities::CalculateCurrent(std::stod(csvOutput(i-1,6))), ALLOWED_DIGITS));

	  //Save the temperature, pressure, and gapsize values
	  csvOutput(i-1, outputSize.Columns) = std::to_string(Utilities::TruncateValue(systemTemperature, ALLOWED_DIGITS));
	  csvOutput(i-1, outputSize.Columns+1) = std::to_string(Utilities::TruncateValue(systemPressure, ALLOWED_DIGITS));
	  csvOutput(i-1, outputSize.Columns+2) = std::to_string(Utilities::TruncateValue(systemGapScaled, ALLOWED_DIGITS));

	  //Calculate control ratio
	  csvOutput(i-1, outputSize.Columns+3) = std::to_string(Utilities::TruncateValue(Utilities::CalculateNormalizedControl(longElectrodeValue, shortElectrodeValue), ALLOWED_DIGITS_CONTROL_RATIO));
	  
	  //Calculate diluted analyte concentration in the device
	  csvOutput(i-1, 24) = std::to_string(Utilities::TruncateValue(Utilities::CalculateAnalyteConcentration(std::stod(csvOutput(i-1,27)),std::stod(csvOutput(i-1,26)),std::stod(csvOutput(i-1,24))), ALLOWED_DIGITS));

	  if(!cli.Present(NO_DOPANT_COLUMN_PRESENT)){
	    //Calculate diluted analyte concentration in the device
	    csvOutput(i-1, 29) = std::to_string(Utilities::TruncateValue(Utilities::CalculateAnalyteConcentration(std::stod(csvOutput(i-1,27)),std::stod(csvOutput(i-1,26)),std::stod(csvOutput(i-1,29))), ALLOWED_DIGITS));	    
	  }
	  
	  //Increment counter
	  rowsDeleted++;
	}

	if(verbose){
	  std::cout << "Relative calculations complete." << std::endl;
	}
	
	//Refresh output file to account for new headers
	csvOutput.Write();
	csvOutput.Read();
	outputSize = csvOutput.Size();	

	//First row already copied, so for every row after first 
	for(unsigned i = 2, tgt = 3; tgt < outputSize.Rows; i++, tgt += 2){
	  //For all columns
	    for(unsigned j = 0; j < outputSize.Columns; j++){
	      //Move target row to current open row
	      csvOutput(i,j) = csvOutput(tgt,j);

	      //Delete old value
	      csvOutput(tgt,j) = "";
	    }
	}
	
	if(verbose){
	  std::cout << "Output lines compressed." << std::endl;
	}
	
	//Remove bad columns and perform secondary calculations
	std::vector<std::string> outputColumns = csvOutput.ColumnHeaders();
	std::vector<bool> deleteColumn;

	//Determine which columns should be deleted
	for(unsigned i = 0; i < outputColumns.size(); i++){
	  deleteColumn.push_back(saveAllColumns && MarkColumnDeleted(outputColumns[i]));

	  //If column is to be deleted
	  if(deleteColumn[i]){
	    //Increment counter
	    columnsDeleted++;
	    
	    //If verbose
	    if(verbose){
	      //Print out message indicating column is being deleted
	      std::cout << "Marking '" << outputColumns[i] << "' for deletion." << std::endl;
	    }
	  }
	}

	//For every column
	for(unsigned i = 0; i < outputSize.Columns; i++){
	  //If it is marked for deletion
	  for(unsigned j = 0; deleteColumn[i] && (j < outputSize.Rows); j++){
	    //Write a blank string into every row
	    csvOutput(j, i) = "";
	  }
	}

	//For every column
	for(unsigned i = 0, deleted = 0, tgt = 0; (i + deleted) < outputSize.Columns; i++){
	  //Reset target to current index
	  tgt = i;
	  
	  //If the column header is blank
	  if(csvOutput(0,i) == ""){
	    //Find the next non-blank column to copy
	    for(unsigned j = i+1; j < outputSize.Columns; j++){
	      //If current column is not blank
	      if(csvOutput(0,j) != ""){
		//Update target index
		tgt = j;

		//Update loop index to jump out
		j = outputSize.Columns;
	      }
	    }

	    //If an appropriate target was found
	    if(tgt > i){
	      //For every column
	      for(unsigned col = tgt, offset = 0; col < outputSize.Columns; col++, offset++){
		//For every row
		for(unsigned row = 0; row < outputSize.Rows; row++){
		  //Copy the current column value to the current column
		  csvOutput(row, i + offset) = csvOutput(row, col);

		  //Remove the original value
		  csvOutput(row, col) = "";
		}
	      }
	    }
	    
	    //Increment the deleted counter by the amount of rows moved
	    deleted += (tgt - i);	    
	  }
	}

	//Update CSV output size
	csvOutput.Size(outputSize.Rows - rowsDeleted, outputSize.Columns - columnsDeleted);
		       
	//Save contents to file
	csvOutput.Write();
	
	//If flag is present, remove baseline
	if(cli.Present(BASELINE_REMOVAL)){
	  //Read the CSV baseline file
	  CommaSeparatedValues csvBaseline = CommaSeparatedValues(cli.Get(BASELINE_REMOVAL));
	  csvBaseline.Read();

	  //Get the column header mapping for the baseline file
	  std::map<std::string, unsigned> baselineHeaderMapping = csvBaseline.ColumnMapping();

	  if(verbose){
	    std::cout << "Mapping columns in baseline file:" << std::endl;
	  }
	  
	  //Ensure CSV output file object contents are updated
	  csvOutput.Read();
	  
	  //Get current output size
	  outputSize = csvOutput.Size();
	  
	  Utilities::ConvertedData dataValue, baselineValue;
	  csvOutput(0, outputSize.Columns) = "Baseline";

	  //For every column in the output file
	  for(unsigned j = 0; j < outputSize.Columns; j++){

	    if(verbose){
	      std::cout << "Data Column " << j << " '" << csvOutput(0,j) << "' -> Baseline Column " << baselineHeaderMapping[csvOutput(0,j)] << std::endl; 
	    }
	    
	    //If the current column is a detector output
	    if(csvOutput(0,j).find("Detector") != std::string::npos){

	      if(verbose){
		std::cout << "Attempting to remove baseline from '" << csvOutput(0,j) << "' column of datafile." << std::endl;
	      }
	      
	      //For every row after the header	    
	      for(unsigned i = 1; i < outputSize.Rows; i++){
		
		//Convert data and baseline values
		dataValue = Utilities::ConvertValue_Double(csvOutput(i,j));
		baselineValue = Utilities::ConvertValue_Double(csvBaseline(i,baselineHeaderMapping[csvOutput(0,j)]));
		
		//If neither conversion resulted in an error
		if(!dataValue.error && !baselineValue.error){
		  //Subtract baseline value from data value
		  csvOutput(i, j) = std::to_string(Utilities::TruncateValue(dataValue.value - baselineValue.value, ALLOWED_DIGITS));

		  //If the baseline flag has not been written
		  if(csvOutput(i, outputSize.Columns) == ""){
		    //Set the removed value
		    csvOutput(i, outputSize.Columns) = "REMOVED";
		  }
		}
		else{
		  //Print an error to the console indicating something happened
		  std::cerr << "An error resulted attempting to convert '" << csvOutput(0,j) << "' value into a double for row " << i << ". Baseline not removed." << std::endl;
		  std::cerr << "Data file message: " << dataValue.msg << std::endl << "Baseline file message " << baselineValue.msg << std::endl;
		  csvOutput(i, outputSize.Columns) = "PRESENT";
		}
	      }
	    }
	  }
	}
	
	//Write contents back to disk
	csvOutput.Write();
      }
    }
    //Indicate which file had trouble
    else{
      std::cerr << "Unable to open ";
      
      if(!inputFlag && outputFlag){
	std::cerr << "input file.";
	outputFile.close();
      }
      else if(inputFlag && !outputFlag){
	std::cerr << "output file.";
	inputFile.close();
      }
      else{
	std::cerr << "input and output files.";
      }

      std::cerr << std::endl;
    }

    
  }
    
  return 0;
}
