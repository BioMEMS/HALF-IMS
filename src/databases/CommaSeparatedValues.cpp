#include "CommaSeparatedValues.h"

CommaSeparatedValues(string filename) : SeparatedValues(filename){
  //Set the file delimiter to a comma
  this.fileDelimiter = ',';

  return;
}

