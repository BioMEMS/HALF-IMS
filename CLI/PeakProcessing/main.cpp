//Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>

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
CommaSeparatedValues* ProcessFile(std::string file, double aperture, std::vector<std::string> dataHeaders, std::vector<std::string> parameterHeaders){
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
  std::vector<std::string>* dataStrings;
  double* dataValues = new double[size.Rows-1];
  
  //For each column
  for(unsigned i = 0; i < size.Columns; i++){
    //Get the header name
    headerColumn = data(0,i);

    //Put header in output CSV
    (*processed)(0,i) = headerColumn;

    //If the column is a data or parameter column
    if(ProcessHeader(headerColumn, dataHeaders) || ProcessHeader(headerColumn, parameterHeaders)){
      //Transpose the CSV to turn rows into columns
      data.Transpose(true, false);

      //Extract a column, previously a row, from the CSV
      dataStrings = data[i];

      //Undo the transpose
      data.Transpose(false, false);
      
      //For all the data strings other than the header value
      for(unsigned j = 1; j < size.Rows; j++){
	//If the current string can be converted to a numerical value
	if(numChecker.NumericalConvert((*dataStrings)[j])){
	  //Convert value and place in values array
	  dataValues[j-1] = std::stod((*dataStrings)[j]);
	  
	  //If the column is a data header
	  if(ProcessHeader(headerColumn, dataHeaders)){
	  
	    //Normalize value by 2.5 V and invert sign
	    dataValues[j-1] = -1*(dataValues[j-1] - 2.5);
	  }
	}
	//Otherwise
	else{
	  //Write a default value of zero
	  dataValues[j] = 0;
	}
      }
      
      //Filter column
      //filter.Apply(&dataValues, SignalProcessing::Filter::Operation::WeightedAverage);

      //For every value after the column header
      for(unsigned j = 1; j < size.Rows; j++){
	//Convert data into a string and value into CSV file
	(*processed)(j, i) = std::to_string(dataValues[j]);
      }

      //Clean up memory after done
      if(dataStrings != nullptr){
	delete dataStrings;
      }
    }
    else{
      
    }
    
  }

  //De-allocate heap memory
  delete[] dataValues;
  
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
  double maximum = -100; //Default values should be well below anything in the files
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
	else{
	  maximum += detVal;
	}
      }

      //If the values found were for a data header
      if(dataHeader){
	//Convert maximum found value to a string and place in output
	(*output)(outputSize.Rows, outputColumns[temp]) = std::to_string(maximum);
	
	//Calculat the time difference and place in output
	(*output)(outputSize.Rows, outputColumns[temp]+1) = std::to_string(std::stod((*data)(stopIndex, timeAxisIndex)) - std::stod((*data)(startIndex, timeAxisIndex)));				     
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
  double aperture = 20;
  
  //Instantiate command line input parser
  Utilities::CLIParser cli;

  //Calculate flag types to be accepted
  int flagTypeOne = Utilities::CLIParser::Dash | Utilities::CLIParser::Space;
  int flagTypeTwo = Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Space;
  int flagTypeThree = Utilities::CLIParser::Dash | Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Standalone;

  //Add desired flags to parser
  cli.Add(HELP_NAME, std::vector<std::string>{"h", "help"}, std::vector<int>{flagTypeThree});
  cli.Add(INPUT_FILES, std::vector<std::string>{"i", "input"}, std::vector<int>{flagTypeOne, flagTypeTwo});
  cli.Add(OUTPUT_DIRECTORY, std::vector<std::string>{"o", "output"}, std::vector<int>{flagTypeOne, flagTypeTwo});
  cli.Add(AVERAGE_VALUE, std::vector<std::string>{"af", "average-factor"}, std::vector<int>{flagTypeOne, flagTypeTwo});
  cli.Add(DATA_COLUMNS, std::vector<std::string>{"d", "data-columns"}, std::vector<int>{flagTypeOne, flagTypeTwo});
  cli.Add(PARAMETER_COLUMNS, std::vector<std::string>{"p", "parameter-columns"}, std::vector<int>{flagTypeOne, flagTypeTwo});
  cli.Add(TIME_COLUMN, std::vector<std::string>{"t", "time-column"}, std::vector<int>{flagTypeOne, flagTypeTwo});
	  
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
    //Extract the value and convert to an unsigned
    aperture = std::stod(cli.Get(AVERAGE_VALUE));
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
  //If file is blank
  if((peaks->Size()).Rows == 0){
    unsigned offset = 1;
    //Write in the header row beginning with the file column
    (*peaks)(0,0) = "File";
    outputIndices.insert({"File", 0});
    
    //For all parameter columns
    for(unsigned i = 0; i < parameterColumns.size(); i++){
      //Write desired parameter column
      (*peaks)(0,i+offset) = parameterColumns[i];
      //Add index name to the referential array
      outputIndices.insert({parameterColumns[i], i+offset});
    }
    
    //Update offset value
    offset = parameterColumns.size()-1;

    //For all data columns
    for(unsigned i = 0, j=0; j < dataColumns.size(); j++,i+=2){
      //Write rise time column header for data column
      (*peaks)(0, i+dataColumns.size()+offset) = dataColumns[j] + " Rise Time";
      //Write maximum column header for data column
      (*peaks)(0, i+dataColumns.size()+offset+1) = dataColumns[j] + " Max";
      
      //Add single data column to referential array
      outputIndices.insert({dataColumns[j], i + dataColumns.size() + offset});
    }
  }

  peaks->Write();

  //Push time column onto parameter columns as last element to ensure
  //it is always included in processed CSV files, but can be easily found
  //for peak data generation. Do this after generating the header for
  //the peak data file to prevent weirdness
  parameterColumns.push_back(timeColumn);
  
  //For every file opened
  for(unsigned i = 0; i < inputFiles.size(); i++){
    files[i] = ProcessFile(inputFiles[i], aperture, dataColumns, parameterColumns);
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
