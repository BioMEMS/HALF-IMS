//Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <thread>

//HALF-IMS libraries
#include "Utilities.h"
#include "CommaSeparatedValues.h"

//Pre-processor Variables
#define OUTPUT_FLAG "output"
#define INPUT_FLAG "input"
#define HELP_FLAG "help"
#define AVG_FLAG "average"

//Function to load a CSV file and normalize DET1 and DET2 columns to 2.5V and average to an arbitrary value
void ProcessFile(std::string file, unsigned averageFactor, CommaSeparatedValues* files[], unsigned index){

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
      if(temp != "X_VALUE"){
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

  //Place pointer in array
  files[index] = processed;
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
  std::thread* threads[inputFiles.size()];
  
  //For every file input
  for(unsigned i = 0; i < inputFiles.size(); i++){
    threads[i] = new std::thread(ProcessFile, inputFiles[i], averageFactor, files, i);
  }

  for(unsigned i = 0, counter = 0, completed = 0; counter < inputFiles.size(); i++){
    //If not off the array
    if(i <= inputFile.size()){
      //Update counter
      counter += threads[i]->joinable();
    }
    //Otherwise
    else{
      if(counter > completed){
	std::cout << "Progress: " << counter << "/" << inputFiles.size() << std::endl;
	completed = counter;
      }
      i = 0;
      counter = 0;
    }
  }
  
  //For all threads
  for(unsigned i = 0; i < inputFiles.size(); i++){
    //Block this thread until they can be joined
    threads[i]->join();
  }
  
  //For every file opened
  for(int i = 0; i < inputFiles.size(); i++){
    std::cout << "Processing: " << inputFiles[i] << std::endl;
    files[i]->Open(FileToOutputDirectory(inputFiles[i], output));
    files[i]->Write();
    //Delete the allocated memory
    delete files[i];
    delete threads[i];
  }
    
  return 0;
}
