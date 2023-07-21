#include "CommaSeparatedValues.h"

CommaSeparatedValues(std::string filename) : SeparatedValues(filename){
  //Set the file delimiter to a comma
  this.fileDelimiter = ',';

  return;
}

