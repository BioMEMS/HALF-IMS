#include "CommaSeparatedValues.h"

CommaSeparatedValues::CommaSeparatedValues(){

}

CommaSeparatedValues::CommaSeparatedValues(std::string filename) : SeparatedValues(filename){
  //Set the file delimiter to a comma
  SetDelimiter(',');

  return;
}

