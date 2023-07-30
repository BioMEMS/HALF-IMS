//Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

//HALF-IMS libraries
#include "Utilities.h"
#include "CommaSeparatedValues.h"

//Pre-processor Variables
#define OUTPUT_FLAG "output"
#define INPUT_FLAG "input"
#define HELP_FLAG "help"
#define AVG_FLAG "average"

//Function to load a CSV file and normalize DET1 and DET2 columns to 2.5V and average to an arbitrary value
CommaSeparatedValues* ProcessFile(std::string file, unsigned averageFactor){

  //Instantiate an object for the processed data
  CommaSeparatedValues *processed = new CommaSeparatedValues("temp.csv");
  //Get the file data
  CommaSeparatedValues data(file);
  data.Read();

  //Grab the size for loops
  Utilities::Limits size = data.Size();
  
  std::string temp;
  double tempData, average, weight, weightSum;
  
  //For each column
  for(unsigned i = 0; i < size.Columns; i++){
    //Get the header name
    temp = data(0,i);
    //Put header in output CSV
    (*processed)(0,i) = temp;
    
    //If the header is for either detector
    for(unsigned j = 1, start=1, stop=averageFactor+1; j < size.Rows; j++){
      
      //Clear values
      average = 0;
      tempData = 0;
      weight = 1;
      weightSum = 0;
      
      //If the current column is NOT the X-value
      if(temp != "X_Value"){
	//Calculate start and stop indices
	start = j - averageFactor/2;
	stop = j + averageFactor/2;
	
	//If start is below 1
	if(start <= 1){
	  //Set to 1
	  start = 1;
	}
	
	//If stop is above the total row count
	if(stop >= size.Rows){
	  //Set to the last row
	  stop = size.Rows - 1;
	}
	
	//For the entirety of the aperture
	for(unsigned k = start; k < stop; k++){
	  //If the data is not a blank string
	  if(data(j,i) != ""){
	    //Convert the data to a numerical value
	    tempData = std::stod(data(j,i));
	  }
	  //Otherwise
	  else{
	    //Default to zero
	    tempData = 0;
	  }

	  //If the indices are the same
	  if(k == j){
	    //Weight is one
	    weight = 1;
	  }
	  //Otherwise
	  else{
	    //Convert unsigned to integers
	    int cur = j;
	    int tgt = k;
	    //Weight is the inverse of the distance from the target point
	    weight = 1/std::abs(cur-tgt);
	  }
	  
	  //Add the values together
	  average += (weight*tempData);
	  weightSum += weight;
	}
	
	//Calculate the weight average of the point
	average = average/weightSum;
	
	//If DET1 or DET2 columns
	if(temp == "DET1" || temp == "DET2"){
	  //Normalize to 2.45 V and flip sign
	  average = -1*(average-2.45);

	}

	//Convert average to a string and save to CSV
	(*processed)(j,i) = std::to_string(average);
      }
      //Otherwise
      else{
	//Copy string value directly and do not smooth
	(*processed)(j,i) = data(j,i);
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
  (*output)(outputSize.Rows, 5) = std::to_string(std::stod((*data)(colIndices[2],stopIndices[0])) - std::stod((*data)(colIndices[2], startIndices[0])));
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
  unsigned averageFactor = 20;
  
  //Instantiate command line input parser
  Utilities::InputFlags cli;

  //Calculate flag types to be accepted
  int flagTypeOne = Utilities::InputFlags::Dash | Utilities::InputFlags::Space;
  int flagTypeTwo = Utilities::InputFlags::DoubleDash | Utilities::InputFlags::Space;
  int flagTypeThree = Utilities::InputFlags::Dash | Utilities::InputFlags::DoubleDash | Utilities::InputFlags::Standalone;

  //Add desired flags to parser
  cli.Add(HELP_FLAG, std::vector<std::string>{"h", "help"}, std::vector<int>{flagTypeThree});
  cli.Add(INPUT_FLAG, std::vector<std::string>{"i", "input"}, std::vector<int>{flagTypeOne, flagTypeTwo});
  cli.Add(OUTPUT_FLAG, std::vector<std::string>{"o", "output"}, std::vector<int>{flagTypeOne, flagTypeTwo});
  cli.Add(AVG_FLAG, std::vector<std::string>{"af", "average-factor"}, std::vector<int>{flagTypeOne, flagTypeTwo});
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //If the user called for the help routine
  if(cli.Present(HELP_FLAG)){
    //Print a message and exit
    std::cout << "Help called!" << std::endl;
    return 0;
  }

  //Declare default output as current directory
  std::string output = "."; //Might break on Windows? Use Filesystem library?
  //If the output flag is present
  if(cli.Present(OUTPUT_FLAG)){
    //Extract necessary arguments
    output = cli.Get(OUTPUT_FLAG);
  }
  
  //Instantiate a vector to hold the files to process
  std::vector<std::string> inputFiles;
  if(cli.Present(INPUT_FLAG)){
    inputFiles = GetInputFiles(cli.GetList(INPUT_FLAG), &abort);
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
  if(cli.Present(AVG_FLAG)){
    //Extract the value and convert to an unsigned
    averageFactor = std::stoi(cli.Get(AVG_FLAG));
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
  }

  peaks->Write();
  
  //For every file opened
  for(unsigned i = 0; i < inputFiles.size(); i++){
    std::cout << "Processing: " << inputFiles[i] << std::endl;
    files[i] = ProcessFile(inputFiles[i], averageFactor);
    std::cout << "Generating peak data..." << std::endl;
    GeneratePeakData(inputFiles[i], files[i], peaks);
  }
  
  peaks->Write();
  
  for(unsigned i = 0; i < inputFiles.size(); i++){
    //files[i]->Open(FileToOutputDirectory(inputFiles[i], output));
    //files[i]->Write();
    //Delete the allocated memory
    delete files[i];
  }
  delete peaks;
  
  return 0;
}
