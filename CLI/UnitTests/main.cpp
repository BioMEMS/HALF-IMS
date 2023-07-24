//Standard libraries
#include <iostream>
#include <fstream>

//HALF-IMS libraries
#include "SeparatedValues.h"
#include "CommaSeparatedValues.h"

//Global variables for test development
std::vector<std::string> contents = {"one", "two", "three", "four", "five", "six"};
int rows = 2;
int columns = 3;

namespace UnitTestsOutput{
    //Simple function to convert boolean to string with some flavor
    std::string StatusOutput(bool status){
      if(status){
	return "PASS";
      }
      else{
	return "FAIL";
      }
    }

  //Simple function to standardize printing ouputs
  void PrintLine(std::string filename, std::string name, bool status){
    std::cout << filename <<  " Unit Tests (" << name << "): " << StatusOutput(status) << std::endl;
    
    return;
  }
  
}

namespace DatabaseUnitTests{
  
  //All the unit tests for the database classes
  bool SeparatedValuesUnit(){
    bool status = true;

    SeparatedValues temp("temp.sv");
    
    return status;
  }

  bool SeparatedValuesReadFileUnit(){
    bool status = false;

    std::string file = "temp.sv";
    std::fstream tempFile;

    //Open the temporary file for output and truncate the file
    tempFile.open("temp.sv", std::fstream::out | std::fstream::trunc);

    //Write the contents
    for(int i = 0; i < rows; i++){
      for(int j = 0; j < columns; j++){
        tempFile << contents[i*columns + j] << " ";
      }
      tempFile << std::endl;
    }

    //Close the file
    tempFile.close();

    try{
    SeparatedValues temp(file);
    temp.Parse();
    status = true;
    }
    catch(const std::exception& exception){
      //Do nothing
    }
    
    return status;
  }
  
  bool SeparatedValuesReadBadFileUnit(){
    bool status = false;

    try{
      SeparatedValues temp("temp.sv");
      temp.Parse();
      status = true;
    }
    catch(const std::exception& exception){
      //Do nothing
    }
    
    return status;
  }

  bool SeparatedValuesCheckContents(){
    bool status = true;

    std::string file = "temp.sv";
    
    //Open the temporary file for output and truncate the file
    std::fstream tempFile;
    tempFile.open(file, std::fstream::out | std::fstream::trunc);

    //Write the contents
    for(int i = 0; i < rows; i++){
      for(int j = 0; j < columns; j++){
        tempFile << contents[i*columns + j] << " ";
      }
      tempFile << std::endl;
    }

    //Close the file
    tempFile.close();

    try{
      SeparatedValues temp(file);
      temp.Parse();

      for(int i = 0; i < rows; i++){
	for(int j = 0; j < columns; j++){
	  status &= (contents[i*columns + j].compare(temp(i,j)) == 0);
	}
      }
      
    }
    catch(const std::exception& exception){
      status = false;
    }

    return status;
  }
}

int main(int argc, char *argv[]){
  
  UnitTestsOutput::PrintLine("SeparatedValues.cpp", "Blank Constructor", DatabaseUnitTests::SeparatedValuesUnit());
  UnitTestsOutput::PrintLine("SeparatedValues.cpp", "File Parse Crash", DatabaseUnitTests::SeparatedValuesReadFileUnit());
  UnitTestsOutput::PrintLine("SeparatedValues.cpp", "Non-existent File Parse Crash", DatabaseUnitTests::SeparatedValuesReadBadFileUnit());
  UnitTestsOutput::PrintLine("SeparatedValues.cpp", "Contents Check", DatabaseUnitTests::SeparatedValuesCheckContents());
  
  return 0;
}
