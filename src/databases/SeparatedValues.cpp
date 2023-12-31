#include "SeparatedValues.h"

//Public Functions
//Class constructor
SeparatedValues::SeparatedValues(){
  this->filename = "";
  this->fileDelimiter = ' ';
  this->readTranspose = false;
  this->writeTranspose = false;
  this->maxRow = 0;
  this->maxColumn = 0;
}

SeparatedValues::SeparatedValues(std::string filename){
  //Store file name within object
  this->filename = filename;

  //Set a space as the file delimiter
  this->fileDelimiter = ' ';
  
  //Set default flags
  this->readTranspose = false;
  this->writeTranspose = false;

  //Set default size parameters
  this->maxRow = 0;
  this->maxColumn = 0;
  return;
}

//Class destructor
SeparatedValues::~SeparatedValues(){
  
  return;
}

std::string SeparatedValues::operator() (unsigned row, unsigned column) const{

  //Declare temporary row and column variables
  unsigned locRow, locColumn;

  //If the tranpose flag is set
  if(readTranspose){
    //Swap row and column
    locRow = column;
    locColumn = row;
  }
  //Otherwise
  else{
    //Use as provided
    locRow = row;
    locColumn = column;
  }
  
  //If the row or column is outside the matrix
  if(locRow >= content.size() || locColumn >= content[locRow].size()){
    //Return a blank
    return "";
  }
  //Otherwise
  else{
    //Return matrix content
    return content[locRow][locColumn];
  }
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

std::unique_ptr<std::vector<std::string>> SeparatedValues::operator[](unsigned row){
  
  unsigned maxIndex = 0;
  //If matrix is transposed
  if(readTranspose){
    //Set the maximum index as the total row count
    maxIndex = content.size();
  }
  //Otherwise
  else{
    //Set the maximum index as the total column count for the desired row
    maxIndex = content[row].size();
  }

  //Allocate enough memory to hold the desired values
  std::unique_ptr<std::vector<std::string>> values = std::make_unique<std::vector<std::string>>(maxIndex);
  //values->resize(maxIndex);
  
  //For every desired item
  for(unsigned i = 0; i < maxIndex; i++){
    //Use the extraction operator to get the row value
    (*values)[i] = operator()(row,i);
  }
  
  //Provide a copy of the vector
  return values;
}

unsigned SeparatedValues::ExtractRow(std::string* output, unsigned row){

  unsigned maxIndex = 0;
  //If matrix is transposed
  if(readTranspose){
    //Set the maximum index as the total row count
    maxIndex = content.size();
  }
  //Otherwise
  else{
    //Set the maximum index as the total column count for the desired row
    maxIndex = content[row].size();
  }

  //Allocate a static array
  output = new std::string[maxIndex];

  //For every desired item
  for(unsigned i = 0; i < maxIndex; i++){
    //Use the extraction operator to get the row value
    output[i] = operator()(row,i);
  }

  
  return maxIndex;
}

void SeparatedValues::Read(){
  
  //Open file stream object if it exists
  if(std::filesystem::exists(filename)){
    file.open(filename, std::fstream::in);
  }
  
  //If the file was actually opened
  if(file.is_open()){

    //Default maximum values to later update
    maxRow = 0;
    maxColumn = 0;

    //Clear out any previous contents
    content.clear();
    
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

	    //If the end of line
	    if(endOfLine){
	      //Revert adjust value to prevent erroneous clipping of last column
	      substringAdjust = 0;
	    }
	    
	    //Slice out a substring
	    temp.push_back(line.substr(start, i-start-substringAdjust));
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

      //If the temporary size is greater than the historical value
      if(temp.size() > maxColumn){
	//Update historical value
	maxColumn = temp.size();
      }
      
      //Push vector onto content matrix
      content.push_back(temp);
    }

    
    //Update maximum rows
    maxRow = content.size();
    
    //If the last line was blank (i.e. only a newline character)
    if(characterCount == 0){
      //Decrement maximum rows by one
      maxRow--;
    }

    //Close file
    file.close();
  }

  return;
}

void SeparatedValues::Write(){
  //Open file stream object creating file as necessary
  file.open(filename, std::fstream::out | std::fstream::trunc);
  
  if(file.is_open()){
    //If transpose set
    unsigned locRow, locColumn;
    if(writeTranspose){
      //Swap the maxima values
      locRow = maxColumn;
      locColumn = maxRow;
    }
    //Otherwise
    else{
      //Use maxima values normally
      locRow = maxRow;
      locColumn = maxColumn;
    }
    
    //For each row
    for(unsigned i = 0, tempRow = 0; i < locRow; i++){
      //For each column
      for(unsigned j = 0, tempCol = 0; j < locColumn; j++){
	//Begin enclosing output in double quotes
	file << '"';

	//If transpose flag is set
	if(writeTranspose){
	  //Swap row and column indices
	  tempRow = j;
	  tempCol = i;
	}
	//Otherwise
	else{
	  //Use as provided
	  tempRow = i;
	  tempCol = j;
	}

	//If the indices have gone off the matrix
	if((i >= content.size()) || (j >= content[i].size())){
	  //Write a blank
	  file << "";
	}
	//Otherwise
	else{
	  //Write content from matrix
	  file << content[tempRow][tempCol];
	}

	//Write end quotation
	file << '"';
	
	//If not the last column
	if(j < (locColumn-1)){
	  //Write delimiter character 
	  file << fileDelimiter;
	}
      }
      
      //End line
      file << std::endl;
    }

    //Close file after writing
    file.close();
  }
  
  return;
}

void SeparatedValues::Open(std::string file){
  this->filename = file;

  //If the previous file is open
  if(this->file.is_open()){
    //Close it
    this->file.close();
  }
  
}

void SeparatedValues::Transpose(bool read, bool write){
  //Update class flags
  readTranspose = read;
  writeTranspose = write;
}

Utilities::Limits SeparatedValues::Size(){

  Utilities::Limits temp;
  
  //If read transpose
  if(readTranspose){
    //Swap rows and columns
    temp.Rows = maxColumn;
    temp.Columns = maxRow;
  }
  //Otherwise
  else{
    //Provide as read
    temp.Rows = maxRow;
    temp.Columns = maxColumn;
  }
  
  return temp;
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
  if(column >= content[row].size()){
    //Extend columns of that row to match
    for(;content[row].size() <= column;){
      content[row].push_back("");
    }
  }

  //If either of the maximums are exceeded, update them
  if(row >= maxRow){
    maxRow = content.size();
  }

  if(column >= maxColumn){
    maxColumn = content[row].size();
  }
  return;
}
