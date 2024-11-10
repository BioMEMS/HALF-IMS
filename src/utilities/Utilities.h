#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <iostream>

namespace Utilities{
  //Simple limits structure to hold values
  struct Limits{
    unsigned Rows;
    unsigned Columns;
    double MinimumValue;
    double MaximumValue;
  };

  struct ConvertedData{
    double value;
    bool error;
    std::string msg;
  };
  
  //Convert provided value
  ConvertedData ConvertValue_Double(std::string item);
}

#endif
