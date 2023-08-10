//Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

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

//Function to load a CSV file and normalize DET1 and DET2 columns to 2.5V and average to an arbitrary value
CommaSeparatedValues* ProcessFile(std::string file, double aperture){
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
  std::vector<std::string> dataStrings;
  std::vector<double> dataValues;
  
  //For each column
  for(unsigned i = 0; i < size.Columns; i++){
    //Get the header name
    headerColumn = data(0,i);
    
    //Put header in output CSV
    (*processed)(0,i) = headerColumn;

    //If the column is not the x-values 
    if(headerColumn != "X_Value"){
      //Set the transpose flag to extract data
      data.Transpose(true, false);
  
      //Extract column
      dataStrings = data[i];

      //Unset transpose flag to read column headers properly
      data.Transpose(false, false);
    
      //Clear out data values
      dataValues.clear();

      //Resize to current column size minus the header
      dataValues.resize(dataStrings.size()-1);

      //For all the data strings other than the header value
      for(unsigned j = 1; j < dataStrings.size(); j++){
	//If the current string can be converted to a numerical value
	if(numChecker.NumericalConvert(dataStrings[j])){
	  //Convert value and place in values array
	  dataValues[j] = std::stod(dataStrings[j]);
	  
	  //If the DET1 or DET2 columns
	  if((headerColumn == "DET1") || (headerColumn == "DET2")){
	    //Normalize value by 2.5 V and invert sign
	    dataValues[j] = -1*(dataValues[j] - 2.5);
	  }
	}
	//Otherwise
	else{
	  //Write a default value of zero
	  dataValues[j] = 0;
	}
      }
      
      //Filter column
      filter.Apply(&dataValues, SignalProcessing::Filter::Operation::WeightedAverage);

      //For every value after the column header
      for(unsigned j = 1; j < dataStrings.size(); j++){
	//Convert data into a string and value into CSV file
	(*processed)(i, j) = std::to_string(dataValues[j]);
      }
    }
    
  }
  
  //Return pointer to CSV
  return processed;
}

void GeneratePeakData(std::string file, CommaSeparatedValues* data, CommaSeparatedValues* output){
  //Get the size of the input data
  Utilities::Limits size = data->Size();
  Utilities::Limits outputSize = output->Size();
  
  std::string temp = "";
  unsigned colIndices[7] = {0, 0, 0, 0, 0, 0, 0};
  unsigned startIndices[2] = {0, 0}; //Hold the 10% of maximum points
  unsigned stopIndices[2] = {0, 0}; //Hold the 90% of maximum points
  double maximums[2] = {-100, -100}; //Default values should be well below anything in the files
  double detVal = 0;
  
  //Find the relevant column indices
  for(unsigned i = 0; i < size.Columns; i++){
    temp = (*data)(0,i);
    if(temp == "DET1"){
      colIndices[0] = i;
    }
    else if(temp == "DET2"){
      colIndices[1] = i;
    }
    else if(temp == "X_Value"){
      colIndices[2] = i;
    }
    else if(temp == "VS+"){
      colIndices[3] = i;
    }
    else if(temp == "VS-"){
      colIndices[4] = i;
    }
    else if(temp == "VL+"){
      colIndices[5] = i;
    }
    else if(temp == "VL-"){
      colIndices[6] = i;
    }
  }

  //For each detector
  for(unsigned j = 0; j < 2; j++){
    //For each row starting from the end
    for(unsigned i = size.Rows-1; i > 0; i--){
      //Get the current row's detector value
      detVal = std::stod((*data)(i,colIndices[j]));

      //If the detector value exceeds the maximum
      if(detVal > maximums[j]){
	//Save the value
	maximums[j] = detVal;
      }
      
      //If the detector value is around 10% of the maximum
      if((detVal >= 0.09*maximums[j]) && (detVal <= 0.11*maximums[j])){
	//Save the index
	startIndices[j] = i;
      }
      //If the detector value is around 90% of the maximum
      else if((detVal >= 0.89*maximums[j]) && (detVal <= 0.91*maximums[j])){
	//Save the index
	stopIndices[j] = i;
      }
    }
  }

  //Add the file name to the line
  (*output)(outputSize.Rows, 0) = file;
  
  //Add the long and short voltage values assuming the middle value from the data to be semi-accurate
  (*output)(outputSize.Rows, 1) = (*data)(size.Rows/2,colIndices[3]);
  (*output)(outputSize.Rows, 2) = (*data)(size.Rows/2,colIndices[4]);
  (*output)(outputSize.Rows, 3) = (*data)(size.Rows/2,colIndices[5]);
  (*output)(outputSize.Rows, 4) = (*data)(size.Rows/2,colIndices[6]);

  //Calculate the time difference
  (*output)(outputSize.Rows, 5) = std::to_string(std::stod((*data)(stopIndices[0], colIndices[2])) - std::stod((*data)(startIndices[0], colIndices[2])));
  
  //Write detector maximums to file
  (*output)(outputSize.Rows, 6) = std::to_string(maximums[0]);
  (*output)(outputSize.Rows, 7) = std::to_string(maximums[1]);
  
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
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //If the user called for the help routine
  if(cli.Present(HELP_NAME)){
    //Print a message and exit
    std::cout << "Help called!" << std::endl;
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
  
  //Declare array of CSV files based upon input list
  CommaSeparatedValues* files[inputFiles.size()];

  //Open the desired output file
  CommaSeparatedValues* peaks = new CommaSeparatedValues("");
  peaks->Open(FileToOutputDirectory("peaks.csv", output));
  peaks->Read();
  
  //If file is blank
  if((peaks->Size()).Rows == 0){
    //Write in the header row
    (*peaks)(0,0) = "File";
    (*peaks)(0,1) = "VS+";
    (*peaks)(0,2) = "VS-";
    (*peaks)(0,3) = "VL+";
    (*peaks)(0,4) = "VL-";
    (*peaks)(0,5) = "Rise Time";
    (*peaks)(0,6) = "DET1 Max";
    (*peaks)(0,7) = "DET2 Max";
  }

  peaks->Write();
  
  //For every file opened
  for(unsigned i = 0; i < inputFiles.size(); i++){
    std::cout << "Processing: " << inputFiles[i] << std::endl;
    files[i] = ProcessFile(inputFiles[i], aperture);    
    GeneratePeakData(inputFiles[i], files[i], peaks);
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
