//Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

//HALF-IMS libraries
#include "CommaSeparatedValues.h"

//Function to load a CSV file and normalize DET1 and DET2 columns to 2.5V
CommaSeparatedValues* ProcessFile(std::string file){
  //Get the file data
  CommaSeparatedValues *data = new CommaSeparatedValues(file);
  data->Read();

  //Grab the size for loops
  Utilities::Limits size = data->Size();

  std::string temp;
  //For each column
  for(unsigned i = 0; i < size.Columns; i++){
    //Get the header name
    temp = (*data)(0,i);
    //If the header is for either detector
    if((temp == "DET1") || (temp == "DET2")){
      for(unsigned j = 1; j < size.Rows; j++){
	//Convert string value to a double, normalize to 2.5, invert, and place back into object
	(*data)(j,i) = std::to_string(-1*(std::stod((*data)(j,i)) - 2.5));
      }
    }
  }
  
  return data;
}
  
std::string FileToOutputDirectory(std::string inputFile, std::string outputDirectory){
  std::filesystem::path temp(inputFile);
  
  return outputDirectory + temp.filename().string();
}

int main(int argc, char *argv[]){

  std::string outDir = "./";
  
  //Declare array of CSV files based upon input list
  CommaSeparatedValues* files[argc-1];
  
  //For every file input
  for(int i = 1; i < argc; i++){
    std::cout << "Processing:" << argv[i] << std::endl;
    files[i-1] = ProcessFile(argv[i]);
    files[i-1]->Open(FileToOutputDirectory(argv[i], outDir));
    files[i-1]->Write();
  }

  //For every file opened
  for(int i = 0; i < (argc-1); i++){
    //Delete the allocated memory
    delete files[i];
  }
    
  return 0;
}
