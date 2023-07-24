#include "SeparatedValues.h"

//Public Functions
//Class constructor
SeparatedValues::SeparatedValues(){
  this->filename = "";
  this->fileDelimiter = ' ';
  this->readTranspose = false;
  this->writeTranspose = false;
}

SeparatedValues::SeparatedValues(std::string filename){
  //Store file name within object
  this->filename = filename;

  //Open file stream object if it exists
  if(std::filesystem::exists(filename)){
    file.open(filename, std::fstream::in);
  }

  //Set a space as the file delimiter
  this->fileDelimiter = ' ';
  
  //Set default flags
  this->readTranspose = false;
  this->writeTranspose = false;
  
  return;
}

//Class destructor
SeparatedValues::~SeparatedValues(){
  
  return;
}

std::string SeparatedValues::operator() (unsigned row, unsigned column) const{
  //Declare local row and column variables
  unsigned locRow, locColumn;

  if(readTranspose){
    locRow = column;
    locColumn = row;
  }
  else{
    locRow = row;
    locColumn = column;
  }

  ExpandContent(locRow, locColumn);

  return content[locRow][locColumn];
}

std::string& SeparatedValues::operator()(unsigned row, unsigned column){

  //Declare local row and column variables
  unsigned locRow, locColumn;

  //If the read transpose flag is set
  if(readTranspose){
    //Translate row and column appropriately
    locRow = column;
    locColumn = row;
  }
  //Otherwise
  else{
    //Use indices as provided
    locRow = row;
    locColumn = column;
  }

  ExpandContent(locRow, locColumn);
  
  //Return a pointer to the string
  return content[locRow][locColumn];
}

void SeparatedValues::Parse(){
  //If the file was actually opened
  if(file.is_open()){
    
    std::string line;
    int characterCount = 0;
    bool quoteFound = false, endOfLine = false;

    //While there are contents of the file to read
    while(!file.eof()){
      //Get a line of the file
      std::getline(file, line);
      //Save the number of characters in the line
      characterCount = line.length();

      //Create a temporary row vector
      std::vector<std::string> temp;
      //For every character in the line
      for(int i = 0, start = 0, quoteCounter=0; i < characterCount; i++){

	//Calculate flag for end of line
	endOfLine = (i == (characterCount - 1));
	
	//If the character is the delimiter and an appropriate quote count is found or the end of the line is found
	if((line[i] == fileDelimiter) || endOfLine){
	  //Create local adjustment index
	  int substringAdjust = 0;
	
	  //If the previous character was a valid quote
	  if(quoteFound || (endOfLine && (quoteCounter == 1))){
	    //Set the adjustment to slice out quotation marks
	    substringAdjust = 1;
	  }
	    
	  //If the end of line has been reached, the quote count is zero, or the quote count is two
	  if(endOfLine || (quoteCounter == 0) || (quoteCounter == 2)){
	    //Update start index with substring adjustment for quotations
	    start = start + substringAdjust;
	    //Slice out a substring
	    temp.push_back(line.substr(start, i-start));
	    //Set the start index to the current
	    start = i + 1;
	    //Reset quote counter
	    quoteCounter = 0;
	  }
	}
	    
	//Calculate flag which indicates if a row entry quotation was found
	quoteFound = (line[i] == '"' && ((i==0) || (line[i-1]!='\\')));
	
	//If a quote was found
	if(quoteFound){
	  //Increment counter
	  quoteCounter++;
	}       
      }
      //Push vector onto content matrix
      content.push_back(temp);
    }
  }
}

void SeparatedValues::Transpose(bool read, bool write){
  //Update class flags
  readTranspose = read;
  writeTranspose = write;
}

//Protected functions
void SeparatedValues::SetDelimiter(char delimiter){
  fileDelimiter = delimiter;
}

//Private functions
void SeparatedValues::ExpandContent(unsigned row, unsigned column){

  //If row provided is off the matrix
  if(row >= content.size()){
    //Extend rows to match
    for(unsigned i = 0; i < (row - content.size() + 1); i++){
      std::vector<std::string> blankRow;
      content.push_back(blankRow);
    }
  }

  //If the column provided is off the matrix
  if(Column >= content[row].size()){
    //Extend columns of that row to match
    for(unsigned i = 0; i < (Column - content[row].size() + 1); i++){
      content[row].push_back("");
    }
  }
  
  return;
}
