#ifndef UTILITIES_H
#define UTILITIES_H

namespace Utilities{
  //Simple limits structure to hold values
  struct Limits{
    unsigned Rows;
    unsigned Columns;
    double MinimumValue;
    double MaximumValue;
  };
  
  class InputFlags{
  public:
    InputFlags();
    ~InputFlags();
    
    //Functions to set known flags
  };
}
#endif
