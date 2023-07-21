//Standard libraries
#include <iostream>

//HALF-IMS libraries
#include "SeparatedValues.h"
#include "CommaSeparatedValues.h"

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
  void PrintLine(std::string name, bool status){
    std::cout <<  "Unit Tests (" << name << "): " << StatusOutput(status) << std::endl;
    
    return;
  }
  
}

namespace DatabaseUnitTests{
  //All the unit tests for the database classes
  bool SeparatedValues(){
    bool status = true;
    
    return status;
  }
}

int main(int argc, char *argv[]){
  bool result = false;
  
  UnitTestsOutput::PrintLine("SeparatedValues.cpp", DatabaseUnitTests::SeparatedValues());

  return 0;
}
