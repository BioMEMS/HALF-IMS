#include "Checker.h"

namespace Utilities{
  Checker::Checker(){
    //Blank constructor
  }

  Checker::~Checker(){
    //Blank destructor
  }
  
  bool Checker::NumericalConvert(std::string value){
    //Get length of string
    int length = value.length();
    //Instantiate default flag
    bool flag = (length > 0);

    //If only a single character
    if(length == 1){
      //Check to ensure the only character is not a decimal point
      flag &= (value[0] != '.');
    }
    
    //For each character in the provided string
    for(int i = 0; flag && (i < length); i++){
      //Check for numeric and decimal point characters
      flag &= (value[i] == '.' || isdigit((int)value[i]));
    }

    return flag;
  }
}
