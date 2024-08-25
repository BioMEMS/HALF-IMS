#include "CLIParser.h"

namespace Utilities{
  
  const std::string CLIParser::description = "DESCRIPTION";
  const std::string CLIParser::help = "HELP";
  
  CLIParser::CLIParser(){

  }
  
  CLIParser::~CLIParser(){
    
  }

  void CLIParser::Add(std::string name, std::vector<std::string> flags, std::vector<int> delimiters, std::string description){
    
    //Insert flags under the provided name
    this->flags[name] = flags;
    
    //Insert description under the provided name
    this->descriptions[name] = description;
    
    int temp;
    //For each flag
    for(unsigned i = 0; i < flags.size(); i++){

      //If delimiters were not provided
      if(delimiters.size() == 0){
	//Default to either
	temp = Types::Standalone;
      }
      //If there is not a one-to-one relationship
      else if(i >= delimiters.size()){
	//Use the last delimiter choice
	temp = delimiters[delimiters.size()-1];
      }
      //Otherwise
      else{
	//Pull delimiter directly from list
	temp = delimiters[i];
      }

      //Insert flag with appropriate delimiter
      types[flags[i]] = temp;
      values[flags[i]] =  "";
      present[flags[i]] = false;
    }

    return;
  }

  std::string CLIParser::Get(std::string name){
    //Declare return variable
    std::string value = "";

    //For each flag associated with the name
    for(unsigned i = 0; (value == "") && (i < flags[name].size()); i++){
      //If the flag is present
      if(present[flags[name][i]]){
	//Extract value to return
	value = values[flags[name][i]];
      }
    }

    return value;
  }

  double CLIParser::GetNumeric(std::string name){
    //Insantiate default variables
    double value = std::nan("1");
    Checker checker;
    
    //Get the value found
    std::string temp = this->Get(name);

    //If we can convert to a numerical value
    if(checker.NumericalConvert(temp)){
      //Convert string to a double
      value = std::stod(temp);
    }

    //Return result
    return value;
  }
  
  std::vector<std::string> CLIParser::GetList(std::string name){
    //Get the value string
    std::string temp = this->Get(name);

    //Instantiate return vector
    std::vector<std::string> values;

    //So long as positions can be found
    for(unsigned i = 0, prev = 0, flag = 0; i < temp.length(); i++){
      //Generate a flag to determine if at the end of the string
      flag = (i == (temp.length()-1));
      //If the current character is a comma or at the end of the string
      if(temp[i] == ',' || flag){
	//If right at the end of the list
	if(flag){
	  //Bump the counter by one to not truncate a character
	  i++;
	}
	//Extract the substring and put into vector
	values.push_back(temp.substr(prev, i-prev));
	//Update the history
	prev = i+1; 
      }
    }

    return values;
  }

  std::vector<double> CLIParser::GetNumericList(std::string name){
    //Instantiate return values
    std::vector<double> values;
    Checker checker;
    
    //Get the list of values as strings
    std::vector<std::string> temp = this->GetList(name);

    //For each string value
    for(unsigned i = 0; i < temp.size(); i++){
      //If it can be convereted
      if(checker.NumericalConvert(temp[i])){
	//Convert it to a double
	values.push_back(std::stod(temp[i]));
      }
      //Otherwise
      else{
	//Put a NaN in its place
	values.push_back(std::nan("1"));
      }
    }

    return values;
  }

  void CLIParser::Parse(int count, char* arguments[]){

    std::string locFlag, locValue;
    bool locFound;
    std::vector<std::string> tempList;
    
      //For each flag association
      for(std::map<std::string, std::vector<std::string>>::iterator iter = this->flags.begin(); iter != this->flags.end(); ++iter){
	//Get the flags associated with the key
	tempList = this->flags[iter->first];	

	//For each flag 
	for(unsigned j = 0; j < tempList.size(); j++){
	  locFlag = tempList[j];
	  //Get the type saved
	  int tempType = this->types[locFlag];
	  
	  //Create a space-delimited, equal-delimited regular, and standalone expression
	  std::regex spaceDelimited = this->GenerateRegularExpression(locFlag, tempType & Types::Space);
	  std::regex equalDelimited = this->GenerateRegularExpression(locFlag, tempType & Types::Equal);
	  std::regex soloDelimited = this->GenerateRegularExpression(locFlag, tempType & Types::Standalone);

	  //For each value in the arguments list
	  for(int i = 0; i < count; i++){
	  
	    //Default values
	    locValue = "";
	    locFound = std::regex_match(arguments[i], soloDelimited);
	    
	    //If the current argument matches a space delimited regular expression 
	    if(std::regex_match(arguments[i], spaceDelimited)){
	      locFound = true;
	      //If the current index is not the last one
	      if(i < (count-1)){
		//Get the next value after the flag
		locValue = arguments[i+1];
	      }
	    }
	    //If the current argument matches an equal delimited regular expression
	    else if(std::regex_match(arguments[i], equalDelimited)){
	      locFound = true;
	      //Convert to standard string
	      locValue = arguments[i];
	      //Get all characters after the equal sign
	      locValue = locValue.substr(locValue.find('='));
	    }
	    
	    //If the flag is not currently found
	    if(!this->present[locFlag]){
	      //Update structures with values
	      this->values[locFlag] = locValue;
	      this->present[locFlag] = locFound;
	    }

	  }
	}
      }
      
      return;
  }

  bool CLIParser::Present(std::string name){
    std::vector<std::string> tempFlags = this->flags[name];

    bool status = false;
    
    for(unsigned i = 0; i < tempFlags.size(); i++){
      status |= this->present[tempFlags[i]];
    }
    
    return status;
  }

  bool CLIParser::Help(){
    
    //Instantiate flags indicating message printed and dashes
    bool emptyInput = true, helpPrinted = false;

    //Determine if the default help flag is present
    helpPrinted = this->Present(help);

    //For every element within the flags map so long as a flag was not found
    for(auto it = this->present.begin(); !helpPrinted && emptyInput && (it != this->present.end()); it++){
      //Update help printed flag
      emptyInput &= !it->second;
    }
    
    //If the help flag is present or the flags provided are empty
    if(helpPrinted || emptyInput){

      //If a description is present
      if(this->Present(description)){
	//Output description
	std::cout << this->descriptions[description] << std::endl << std::endl;
      }

      //Print out arguments header
      std::cout << "Options/Arguments" << std::endl;
      
      //For every key in the descriptions mapping
      for(auto it = this->descriptions.begin(); it != this->descriptions.end(); ++it){
	
	//If the key is not the description
	if(it->first != description){
	  //Tab over
	  std::cout << ' ';
	  
	  //Print a UNIX-style description line
	  for(unsigned i = 0, size = this->flags[it->first].size(); i < size; i++){

	    //Print the appropriate number of dashes depending upon the flags
	    for(unsigned dashes = this->types[this->flags[it->first][i]] & (Types::Dash | Types::DoubleDash); dashes != 0; dashes = (dashes >> 1) & (Types::Dash | Types::DoubleDash)){	      
	      std::cout << '-';
	    }

	    //Print out flag
	    std::cout << this->flags[it->first][i];

	    //If not the last flag
	    if((i+1) < size){
	      //Separate by slash
	      std::cout << '/';
	    }
	  }

	  //Print out descript separated by a tab
	  std::cout << "\t" << it->second << std::endl;
	}
      }

      //Separate next line
      std::cout << std::endl;
    }
    
    return helpPrinted || emptyInput;
  }
  
  std::regex CLIParser::GenerateRegularExpression(std::string flag, int type){
    int indexType = Types::Standalone;
    int dashType = type & (Types::Dash | Types::DoubleDash);

    //Begin pattern by assuming that it must be at the beginning of the string
    std::string pattern = "^";
    
    //If not in the error state
    if((type & Types::Space) || (type & Types::Equal) || (type & Types::Standalone)){
      //Bit-mask out index flag
      indexType = type & (Types::Space | Types::Equal | Types::Standalone);
    
      //Generate the dash portion
      pattern += "-";
      //If only a single dash
      if(dashType == Types::Dash){
	//Ensure pattern matches one
	pattern += "{1}";
      }
      //If only a double dash
      else if(dashType == Types::DoubleDash){
	//Ensure pattern matches two
	pattern += "{2}";
      }
      //Otherwise
      else{
	//Match either one or two
	pattern += "{1,2}";
      }
      
      //Place flag after dash section
      pattern += flag;
      
      //If the index type is an equals
      if(indexType == Types::Equal){
	//Ensure that the 
	pattern += "=.*";
      }
    }
    
    //Ensure the pattern matches the entirety of the string
    pattern += "$";
    
    return std::regex(pattern);
  }

  //End of Utilities namespace
}

