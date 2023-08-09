#ifndef CHECKER_H
#define CHECKER_H

#include <string>

namespace Utilities{
  
  class Checker{
  public:
    Checker();
    ~Checker();
    
    /*
      Name:    NumericalConvert
      Purpose: Determine if a string can be converted to a numerical value.
      Inputs:  value (std::string) - The string to check.
      Outputs: flag (bool) - Indicator if the string can be converted.
      Notes:   None
    */
    bool NumericalConvert(std::string value);

  };
}

#endif
