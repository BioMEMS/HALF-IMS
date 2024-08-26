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

//Pre-processor Variables
#define OUTPUT_DIRECTORY "output"
#define INPUT_FILES "input"
#define HELP_NAME "help"
#define AVERAGE_VALUE "average"
#define AVERAGE_REPEATS "repeats"
#define DATA_COLUMNS "data"
#define PARAMETER_COLUMNS "parameters"
#define TIME_COLUMN "sample"

bool ProcessHeader(std::string target, std::vector<std::string> headers){
  bool flag = false;

  //Check all headers against target
  for(unsigned i = 0; !flag && i < headers.size(); i++){
    flag |= (headers[i] == target);
  }

  return flag;
}

//Function to load a CSV file and normalize DET1 and DET2 columns to 2.5V and average to an arbitrary value
CommaSeparatedValues* ProcessFile(std::string file, double aperture, unsigned averageRepeats, std::vector<std::string> dataHeaders, std::vector<std::string> parameterHeaders){
  //Instantiate a filter object
  SignalProcessing::Filter filter;

  //Set the aperture value for the filter
  filter.SetParameter(SignalProcessing::Filter::Parameters::Aperture, aperture);
  
  //Instantiate a checker object
  Utilities::Checker numChecker;
  
  //Instantiate an object for the processed data
  CommaSeparatedValues *processed = new CommaSeparatedValues("temp.csv");

  //Get the file data
  CommaSeparatedValues data(file);
  data.Read();
  
  //Grab the size for loops
  Utilities::Limits size = data.Size();
  
  std::string headerColumn;
  //Dynamically allocate array with same number of elements as the rows
  //std::vector<std::string>* dataStrings;
  unsigned elements = 0;
  std::string* dataStrings;
  std::vector<double>* dataValues = new std::vector<double>(size.Rows-1);

  //For each column
  for(unsigned i = 0; i < size.Columns; i++){
    //Get the header name
    headerColumn = data(0,i);

    //Put header in output CSV
    (*processed)(0,i) = headerColumn;

    //Transpose the CSV to turn rows into columns
    data.Transpose(true, false);
    
    //Extract a column, previously a row, from the CSV
    //dataStrings = data[i];
    elements = data.ExtractRow(dataStrings, i);
    
    //Undo the transpose
    data.Transpose(false, false);
    
    //If the column is a data or parameter column
    if(ProcessHeader(headerColumn, dataHeaders) || ProcessHeader(headerColumn, parameterHeaders)){
      
      //For all the data strings other than the header value
      for(unsigned j = 1; j < size.Rows; j++){
	//If the current string can be converted to a numerical value
	if(numChecker.NumericalConvert((*dataStrings)[j])){
	  //Convert value and place in values array
	  (*dataValues)[j-1] = std::stod((*dataStrings)[j]);
	  
	  //If the column is a data header
	  if(ProcessHeader(headerColumn, dataHeaders)){
	  
	    //Normalize value by 2.5 V and invert sign
	    (*dataValues)[j-1] = -1*((*dataValues)[j-1] - 2.5);
	  }
	}
	//Otherwise
	else{
	  //Write a default value of zero
	  (*dataValues)[j] = 0;
	}
      }

      //For the desired number of filtering repeats
      for(unsigned j = 0; j < averageRepeats; j++){
	//Filter column
	filter.Apply(dataValues, SignalProcessing::Filter::Operation::WeightedAverage);
      }
      
      //For every value after the column header
      for(unsigned j = 1; j < size.Rows; j++){
	//Convert data into a string and value into CSV file
	(*processed)(j, i) = std::to_string((*dataValues)[j]);
      }
    }
    //Otherwise
    else{
      
      //For all strings in the column
      for(unsigned j = 1; j < size.Rows; j++){
	//Write to output file
	(*processed)(j, i) = (*dataStrings)[j];
      }
    }
 
    //Clean up memory after done
    if(dataStrings != nullptr){ 
      //delete dataStrings;
      //dataStrings = nullptr;
    }
  }

  //De-allocate heap memory
  delete dataValues;
  
  //Return pointer to CSV
  return processed;
}

void GeneratePeakData(std::string file, CommaSeparatedValues* data, CommaSeparatedValues* output, std::vector<std::string> dataHeaders, std::vector<std::string> parameterHeaders, std::map<std::string, unsigned> outputColumns){
  //Get the size of the input data
  Utilities::Limits size = data->Size();
  Utilities::Limits outputSize = output->Size();

  //Flags to track if the header should be processed
  bool dataHeader = false, parameterHeader = false;
  
  std::string temp = "";
  unsigned startIndex = 0; //Hold the 10% of maximum points
  unsigned stopIndex = 0; //Hold the 90% of maximum points
  unsigned timeAxisIndex = 0; //Hold the index for the time axis for later calculations
  double maximum = 0; 
  double detVal = 0;
  //Get the time axis from the parameters
  std::string timeAxis = parameterHeaders.back();

  //Add the file name to the line
  (*output)(outputSize.Rows, 0) = file;
  
  //Find the time column index
  for(unsigned j = 0; j < size.Columns; j++){
    if((*data)(0, j) == timeAxis){
      timeAxisIndex = j;
    }
  }
  
  //For each column in the data file
  for(unsigned j = 0; j < size.Columns; j++){
    //Pull out the header for the file
    temp = (*data)(0,j);

    //Calculate header processing flags
    dataHeader = ProcessHeader(temp, dataHeaders);
    parameterHeader = (temp != timeAxis) && ProcessHeader(temp, parameterHeaders);
    
    //If the header should be processed as a data header
    if(dataHeader || parameterHeader){

      //If a data header
      if(dataHeader){
	//Reset maximum value to lowest possible value for the system
	maximum = std::numeric_limits<double>::lowest();
      }
      //Otherwise
      else{
	//Reset maximum to zero
	maximum = 0;
      }
      
      //For each row starting from the end
      for(unsigned i = size.Rows-1; i > 0; i--){
	//Get the current row's detector value
	detVal = std::stod((*data)(i,j));

	//If a data header
	if(dataHeader){
	  //If the detector value exceeds the maximum
	  if(detVal > maximum){
	    //Save the value
	    maximum = detVal;
	  }
	  
	  //If the detector value is around 10% of the maximum
	  if((detVal >= 0.09*maximum) && (detVal <= 0.11*maximum)){
	    //Save the index
	    startIndex = i;
	  }
	  //If the detector value is around 90% of the maximum
	  else if((detVal >= 0.89*maximum) && (detVal <= 0.91*maximum)){
	    //Save the index
	    stopIndex = i;
	  }
	}
	//Otherwise
	else{
	  //Add value to maximum for later averaging
	  maximum += detVal;
	}
      }

      //If the values found were for a data header
      if(dataHeader){
	//Calculate the time difference and place in output
	(*output)(outputSize.Rows, outputColumns[temp]) = std::to_string(std::stod((*data)(stopIndex, timeAxisIndex)) - std::stod((*data)(startIndex, timeAxisIndex)));
	
	//Convert maximum found value to a string and place in output
	(*output)(outputSize.Rows, outputColumns[temp]+1) = std::to_string(maximum);			     
      }
      //If the values found were for a parameter header
      else if (parameterHeader){
	//Calculate the average of the maximum value
	maximum /= (size.Rows - 1);

	//Convert value to a string and write to the output
	(*output)(outputSize.Rows, outputColumns[temp]) = std::to_string(maximum);
      }
    }
  }

  return;
}

std::vector<std::string> GetInputFiles(std::vector<std::string> inputs, bool* abort){
  std::vector<std::string> files;
  
  //For all found files/directories
  for(unsigned i = 0; i < inputs.size(); i++){
    //If the input file does not exist
    if(!std::filesystem::exists(std::filesystem::path(inputs[i]))){
      //Print message
      std::cout << "The provided input '" << inputs[i] << "' does not exist as either a file or directory." << std::endl;
      //Set abort flag
      (*abort) = true;
    }
    else{
      std::filesystem::path tempPath = inputs[i];
      //If the path is a directory
      if(std::filesystem::is_directory(tempPath)){
	//Get all files within the directory
	for(auto const& entry : std::filesystem::recursive_directory_iterator(inputs[i])){
	  //If the entry is not a directory
	  if(!std::filesystem::is_directory(entry)){
	    //Save the filename for later parsing
	    files.push_back((entry.path()).parent_path()/(entry.path()).filename());
	  }
	}
      }
      //Otherwise
      else{
	//Add the file directly to the inputs
	files.push_back(inputs[i]);
      }
    }
  }
  
  return files;
}

std::string FileToOutputDirectory(std::string inputFile, std::string outputDirectory){
  std::filesystem::path temp(inputFile);
  
  return outputDirectory + temp.filename().string();
}

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
  //cli.Add(Utilities::CLIParser::description, std::vector<std::string>{""}, std::vector<int>{flagTypeThree}, "A simple program which can take in multiple CSV files and identify the peak values.");
  cli.Add(HELP_NAME, std::vector<std::string>{"h", "help"}, std::vector<int>{flagTypeThree}, "Display this help message.");
  cli.Add(INPUT_FILES, std::vector<std::string>{"i", "input"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The comma-separated list of input files to process.");
  cli.Add(OUTPUT_DIRECTORY, std::vector<std::string>{"o", "output"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The output directory where to place the processed files.");
  cli.Add(AVERAGE_VALUE, std::vector<std::string>{"af", "average-factor"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The aperture of the moving average. Default is 20.");
  cli.Add(AVERAGE_REPEATS, std::vector<std::string>{"ar", "average-repeat"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The number of times to apply the moving average. Default is 1.");
  cli.Add(DATA_COLUMNS, std::vector<std::string>{"d", "data-columns"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The comma-separated list of headers to process as if they were data. Default is \"DET1\",\"DET2\"");
  cli.Add(PARAMETER_COLUMNS, std::vector<std::string>{"p", "parameter-columns"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The comma-separated list of headers to process as if they were parameters. Default is \"VL+\",\"VL-\",\"VS+\",\"VS-\"");
  cli.Add(TIME_COLUMN, std::vector<std::string>{"t", "time-column"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The header value to interpret as the time axis of the data which is used during the rise time calculation.");
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //If the user called for the help routine
  if(cli.Present(HELP_NAME)){
    //Print a message and exit
    cli.Help();
    return 0;
  }

  //Declare default output as current directory
  std::string output = "."; //Might break on Windows? Use Filesystem library?
  //If the output flag is present
  if(cli.Present(OUTPUT_DIRECTORY)){
    //Extract necessary arguments
    output = cli.Get(OUTPUT_DIRECTORY);
  }
  
  //Instantiate a vector to hold the files to process
  std::vector<std::string> inputFiles;
  if(cli.Present(INPUT_FILES)){
    inputFiles = GetInputFiles(cli.GetList(INPUT_FILES), &abort);
  }
  else{
    std::cout << "Inputs are required." << std::endl;
    abort = true;
  }

  //Determine if the output directory exists
  abort &= std::filesystem::exists(std::filesystem::path(output));
  
  //If abort triggered
  if(abort){
    //Stop
    std::cout << "Failure signal received. Ceasing operation." << std::endl;
    return 1;
  }

  //If the average flag is present
  if(cli.Present(AVERAGE_VALUE)){
    //Extract the value and convert to a numeric
    aperture = std::stod(cli.Get(AVERAGE_VALUE));
  }

  //If the average repeat flag is present
  if(cli.Present(AVERAGE_REPEATS)){
    //Extract the value and convert to a numeric
    filterRepeats = std::stod(cli.Get(AVERAGE_REPEATS));
  }
  
  //Set default columns to process
  std::vector<std::string> dataColumns = {"DET1", "DET2"};
  //If the user specified columns to process
  if(cli.Present(DATA_COLUMNS)){
    //Replace list with user defined values
    dataColumns = cli.GetList(DATA_COLUMNS);
  }

  //Set default columns to ignore
  std::vector<std::string> parameterColumns = {"VL+", "VL-", "VS+", "VS-"};
  //If the user specified ignore columns
  if(cli.Present(PARAMETER_COLUMNS)){
    //Replace list with user defined values
    parameterColumns = cli.GetList(PARAMETER_COLUMNS);
  }

  //Set default time column
  std::string timeColumn = "X_Value";
  //If user specified a different column
  if(cli.Present(TIME_COLUMN)){
    //Update value
    timeColumn = cli.Get(TIME_COLUMN);
  }
  
  //Declare array of CSV files based upon input list
  CommaSeparatedValues* files[inputFiles.size()];

  //Open the desired output file
  CommaSeparatedValues* peaks = new CommaSeparatedValues("");
  peaks->Open(FileToOutputDirectory("peaks.csv", output));
  peaks->Read();

  std::map<std::string, unsigned> outputIndices;

  
  unsigned offset = 1;
  bool blankPeakFile = (peaks->Size()).Rows == 0;
  
  //If file is blank
  if(blankPeakFile){
    //Write in the header row beginning with the file column
    (*peaks)(0,0) = "File";
  }    

  //Add all parameter columns and the file to the indices mapping
  outputIndices.insert({"File", 0});
  for(unsigned i = 0; i < parameterColumns.size(); i++){

    //Add index to mapping
    outputIndices.insert({parameterColumns[i], i+offset});
    
    //If the output peak file is blank
    if(blankPeakFile){
      //Write desired parameter column
      (*peaks)(0,i+offset) = parameterColumns[i];
    }
  }
    
  //Update offset value
  offset = dataColumns.size() + parameterColumns.size()-1;
  
  //For all data columns
  for(unsigned i = 0, j=0; j < dataColumns.size(); j++,i+=2){

    //Add single entry to indices mapping
    outputIndices.insert({dataColumns[j], i + offset});

    //If the output peak file is blank
    if(blankPeakFile){
      //Write rise time column header for data column
      (*peaks)(0, i+offset) = dataColumns[j] + " Rise Time";
      //Write maximum column header for data column
      (*peaks)(0, i+offset+1) = dataColumns[j] + " Max";
    }
  }
  
  //For every file opened
  for(unsigned i = 0; i < inputFiles.size(); i++){
    //Process the desired data and parameter columns within the file
    files[i] = ProcessFile(inputFiles[i], aperture, filterRepeats, dataColumns, parameterColumns);
  }

  //Push time column onto parameter columns as last element to ensure
  //it is always included in processed CSV files, but can be easily found
  //for peak data generation. Do this after generating the header for
  //the peak data file to prevent the time axis from being filtered
  parameterColumns.push_back(timeColumn);
  
  //For every file opened
  for(unsigned i = 0; i < inputFiles.size(); i++){
    //Generate peak data for all the parameter and data columns
    GeneratePeakData(inputFiles[i], files[i], peaks, dataColumns, parameterColumns, outputIndices);    
  }
  
  peaks->Write();

  //Cleanup
  for(unsigned i = 0; i < inputFiles.size(); i++){
    files[i]->Open(FileToOutputDirectory(inputFiles[i], output));
    files[i]->Write();
    //Delete the allocated memory
    delete files[i];
  }
  delete peaks;
  
  return 0;
}
