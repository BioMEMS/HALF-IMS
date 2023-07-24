#include "CommaSeparatedValues.h"

CommaSeparatedValues::CommaSeparatedValues(){
  SetDelimiter(',');

  return;
}

CommaSeparatedValues::CommaSeparatedValues(std::string filename) : SeparatedValues(filename){
  //Set the file delimiter to a comma
  SetDelimiter(',');

  return;
}

CommaSeparatedValues::~CommaSeparatedValues(){
  return;
}
