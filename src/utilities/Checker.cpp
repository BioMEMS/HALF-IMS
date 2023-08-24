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
    int start = 0;
    //Instantiate default flag
    bool flag = (length > 0);

    //If only a single character
    if(length == 1){
      //Check to ensure the only character is not a decimal point
      flag &= (value[0] != '.');
    }

    //If the first character is a negative or positive symbol 
    if(flag && ((value[0] == '-') || (value[0] == '+'))){
      //Move the start by one to prevent parsing issues
      start = 1;
    }
    
    //For each character in the provided string
    for(int i = start; flag && (i < length); i++){
      //Check for numeric and decimal point characters
      flag &= ((value[i] == '.') || isdigit((int)value[i]));
    }

    return flag;
  }
}
