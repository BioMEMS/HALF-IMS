#include SeparatedValues.h

//Class constructors
SeparatedValues::SeparatedValues(string filename){
  //Store file name within object
  this.filename = filename;

  //Open file stream object if it exists
  if(std::filesystem::exists(filename)){
    this.file.open(filename);
  }
  else{
    this.file = nullptr;
  }
  
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
  //If the file was actually opened
  if(this.file != nullptr){
    
  }
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
