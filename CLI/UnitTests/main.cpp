#include "SeparatedValues.h"
#include "CommaSeparatedValues.h"

//Simple function to convert boolean to string with some flavor
string StatusOutput(bool status){
  if(status){
    return "PASS";
  }
  else{
    return "FAIL";
  }
}

//All the unit tests for the database classes
bool DatabaseUnitTests(){
  bool status = true;
  
  return status;
}

void main(int argv, char* argc){
  bool result = false;
  
  std::cout << "Database Unit Tests: " << StatusOutput(DatabaseUnitTests()) << std::endl;

  return;
}
