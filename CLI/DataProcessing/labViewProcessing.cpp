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
#define RELATIVE_CALCULATION "rel_calc"
#define BASELINE_REMOVAL "baseline_removal"
#define SYSTEM_PRESSURE "pressure"
#define SYSTEM_TEMPERATURE "temperature"
#define LABVIEW_DATA_COLUMNS 7
#define LABVIEW_TIME_COLUMN_INDEX 0
#define CSV_SHUTTER_COLUMN_INDEX 11
#define CSV_DET_ONE_COLUMN_INDEX 5
#define CSV_DET_TWO_COLUMN_INDEX 6

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

  return results;
}

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

  //Declare default output as current directory
  std::string output, input;
  bool inputFlag, outputFlag, verbose;
  unsigned sampleCompressionMaximum = 4, chemicalIndex = 23;
  double systemTemperature = 275, systemPressure = 26;
  
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
  cli.Add(CHEMICAL_COLUMN_INDEX, std::vector<std::string>{"c", "chemical-column"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The column index which has the chemical name. Default is 23.");
  cli.Add(RELATIVE_CALCULATION, std::vector<std::string>{"nrc", "no-relative-calculation"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "Stop the execution of the relative calculation step.");
  cli.Add(BASELINE_REMOVAL, std::vector<std::string>{"b", "baseline"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "A CSV file which will be used to remove the baseline from the data. Must have equivalent columns to target data file.");
cli.Add(SYSTEM_TEMPERATURE, std::vector<std::string>{"t", "temperature"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The carrier gas temperature, in Kelvin, to assume if not provided in the data file. Default is " + std::to_string(systemTemperature));
 cli.Add(SYSTEM_PRESSURE, std::vector<std::string>{"p", "pressure"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The chip pressure, in PSI, to assume if not provided in the data file. Default is " +std::to_string(systemPressure) + ".");
  
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

      outputFile << "Chemical,Analyte Concentration (ppm),Syringe Volume (mL),Syringe Pump (mL/hr),MFC Setting (mL/min),Long Electrode Setting (V),Short Electrode Setting (V),Detector 1 Current (pA),Detector 2 Current(pA)" << std::endl;
      
      //Read the file until the LabView header line is found
      unsigned lineCount = 0;
      for(std::string line = "", column = ""; !inputFile.eof() && (column != "X_Value"); std::getline(inputFile, line), column = line.substr(0,7), lineCount++){}

      std::vector<std::string> splitLine;
      std::vector<double> avgLine, numericLine;
            
      //For each line of the input file
      double avgCount = -1;
      double timeSegmentCompressionCount = 0;
      for(std::string line = "", convertedVal="", chemical=""; !inputFile.eof(); std::getline(inputFile, line), lineCount++){	
	//Split the line into numeric values
	splitLine = SplitLine(line);
	numericLine = ConvertLine(lineCount, splitLine, chemicalIndex);
	
	//Initialize average line with zeroes
	for(unsigned i = avgLine.size(); i < splitLine.size(); i++){
	  avgLine.push_back(0.0);
	}

	//If line has at least 24 elements
	if(splitLine.size() >= 24){
	  //Grab the 24th element which should be the chemical name	  
	  chemical = splitLine[23];
	}

	//If line exceeds LAB View column count
	if(splitLine.size() > LABVIEW_DATA_COLUMNS){
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

	    //Convert value to a string and append to output line
	    if(i == 23){
	      convertedVal = chemical;
	    }
	    else{
	      convertedVal = std::to_string(trunc(avgLine[i]*100)/100);
	    }
	    line = line + convertedVal + ',';	      

	    //Reset value to zero
	    avgLine[i] = 0.0;
	  }

	  //Add newline character
	  line += '\n';

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
	//Open output file as a CSV
	CommaSeparatedValues csvOutput = CommaSeparatedValues(output);
	csvOutput.Read();
	
	//Get size of output for indexing
	Utilities::Limits outputSize = csvOutput.Size();

	//For every other row in the output file
	for(unsigned i = outputSize.Rows - 1; i > 1; i-=2){
	  //For every column in the pair of rows
	  for(unsigned j = 0; j < outputSize.Columns; j++){
	    //If either of the detector columns
	    if((j == CSV_DET_ONE_COLUMN_INDEX) || (j == CSV_DET_TWO_COLUMN_INDEX)){
	      //Subtract background detector measurement from analyte detector measurement
	      csvOutput(i-1, j) = std::to_string(std::abs(std::stod(csvOutput(i-1,j)) - std::stod(csvOutput(i,j))));
	    }
	    //If the time column index
	    else if (j == LABVIEW_TIME_COLUMN_INDEX){
	      //Copy value
	      csvOutput(i-1, j) = csvOutput(i,j);
	    }
	    
	    //Delete value from column
	    csvOutput(i, j) = "";
	  }
	  
	  //Calculate long and short electrode voltage settings and ideal detector current 	  
	  csvOutput(i-1, outputSize.Columns-4) = std::to_string(std::stod(csvOutput(i-1,7)) - std::stod(csvOutput(i-1,9)));
	  csvOutput(i-1, outputSize.Columns-3) = std::to_string(std::stod(csvOutput(i-1,10)) - std::stod(csvOutput(i-1,8)));
	  csvOutput(i-1, outputSize.Columns-2) = std::to_string((1E12)*Utilities::CalculateCurrent(std::stod(csvOutput(i-1,5))));
	  csvOutput(i-1, outputSize.Columns-1) = std::to_string((1E12)*Utilities::CalculateCurrent(std::stod(csvOutput(i-1,6))));
	  
	}

	//First row already copied, so for every row after first 
	for(unsigned i = 2, tgt = 3; i < (outputSize.Rows - 1); i++, tgt += 2){
	  //For all columns
	    for(unsigned j = 0; j < outputSize.Columns; j++){
	      //Move target row to current open row
	      csvOutput(i,j) = csvOutput(tgt,j);

	      //Delete old value
	      csvOutput(tgt,j) = "";
	    }	    
	}
	
	//Remove bad columns and perform secondary calculations
	std::vector<std::string> outputColumns = csvOutput.ColumnHeaders();
	std::vector<bool> deleteColumn;

	//Determine which columns should be deleted
	for(unsigned i = 0; i < outputColumns.size(); i++){
	  deleteColumn.push_back(false);
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

	//Save contents to file
	csvOutput.Write();
	
	//If flag is present, remove baseline
	if(cli.Present(BASELINE_REMOVAL)){
	  //Read the CSV baseline file
	  CommaSeparatedValues csvBaseline = CommaSeparatedValues(cli.Get(BASELINE_REMOVAL));
	  csvBaseline.Read();

	  //Ensure CSV output file object contents are updated
	  csvOutput.Read();
	  
	  //Get current output size
	  outputSize = csvOutput.Size();
	  
	  Utilities::ConvertedData dataValue, baselineValue;
	  csvOutput(0, outputSize.Columns) = "Baseline";
	  dataValue = Utilities::ConvertValue_Double("0");
	  baselineValue = Utilities::ConvertValue_Double("0");
	  
	  std::vector<unsigned> locDataColumns = { CSV_DET_ONE_COLUMN_INDEX, CSV_DET_TWO_COLUMN_INDEX };
	  //For every row after the header
	  for(unsigned i = 1; i < outputSize.Rows; i++){
	    for(unsigned j = 0; !dataValue.error && !baselineValue.error && (j < locDataColumns.size()); j++){
	      //If both detector columns in the output are not blank
	      if(csvOutput(i, locDataColumns[j]) != ""){
		//Convert data and baseline values
		dataValue = Utilities::ConvertValue_Double(csvOutput(i,locDataColumns[j]));
		baselineValue = Utilities::ConvertValue_Double(csvBaseline(i,locDataColumns[j]));
		
		//If neither conversion resulted in an error
		if(!dataValue.error && !baselineValue.error){
		  //Subtract baseline value from data value
		  csvOutput(i, locDataColumns[j]) = std::to_string(Utilities::CalculateResponse(dataValue.value, baselineValue.value));
		  //Set the removed value		  
		  csvOutput(i, outputSize.Columns) = "REMOVED";
		}
		else{
		  //Print an error to the console indicating something happened
		  std::cerr << "An error resulted attempting to convert Detector " << j + 1 << " value into a double for row " << i << ". ";
		  std::cerr << "Data file message '" << dataValue.msg << "' and baseline file message '" << baselineValue.msg << "' resulted. Baseline not removed." << std::endl;
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
