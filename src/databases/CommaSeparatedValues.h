#ifndef COMMA_SEPARATED_VALUES_H
#define COMMA_SEPARATED_VALUES_H

#include "SeparatedValues.h"

class CommaSeparatedValues : public SeparatedValues {
 public:
  CommaSeparatedValues();
  CommaSeparatedValues(std::string filename);
  ~CommaSeparatedValues();
  
 private:
  
};

#endif
