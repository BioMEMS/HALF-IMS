#include SeparatedValues.h

//Class constructors
SeparatedValues::SeparatedValues(string filename){
  //Store file name within object
  this.filename = filename;
  //Set default flags
  this.transposeFlag = false;
  return;
}

//Class destructor
SeparatedValues::~SeparatedValues(){
  return;
}

//Public Functions
void SeparatedValues::Parse(){
  
}

bool SeparatedValues::Transpose(){
  this.transposeFlag = !this.transposeFlag;
  return this.transposeFlag;
}

//Private Functions
void SeparatedValues::SetDelimiter(char delim){
  this.fileDelimiter = delim;
  return;
}
