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

    std::string line;
    int characterCount = 0;
    bool quoteFound = false, endOfLine = false;
    //While there are contents of the file to read
    while(!this.file.eof()){
      //Get a line of the file
      line = this.file.getline();
      //Save the number of characters in the line
      characterCount = line.length();

      //Create a temporary row vector
      std::vector<string> temp;
      //For every character in the line
      for(int i = 0, start = 0, quoteCounter=0; i < characterCount; i++){

	//Calculate flag for end of line
	endOfLine = i == (characterCount - 1);
	
	//If the character is the delimiter and an appropriate quote count is found or the end of the line is found
	if((line[i] == this.fileDelimiter) || endOfLine){
	  //Create local adjustment index
	  int substringAdjust = 0;

	  //If the previous character was a valid quote
	  if(quoteFound || (endOfLine && (line[i-1] == '"')){
	    //Set the adjustment to slice out quotation marks
	    substringAdjust = 1;
	  }

	  //If the end of line has been reached, the quote count is zero, or the quote count is two
	  if(endOfLine || (quoteCounter == 0) || (quoteCounter == 2)){
	    //Update start index with substring adjustment for quotations
	    start = start + substringAdjust;
	    //Slice out a substring
	    temp.push_back(line.substr(start, i-start-1));
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
      this.content.push_back(temp);
    }
  }
}

void SeparatedValues::Transpose(bool read, bool write){
  //Update class flags
  this.readTranspose = read;
  this.writeTranspose = write;
}
