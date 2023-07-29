#include "Utilities.h"
#include <iostream>

namespace Utilities{


  Checks::Checks(){
    //Blank constructor
  }

  Checks::~Checks(){
    //Blank destructor
  }
  
  bool Checks::NumericalConvert(std::string value){
    //Get length of string
    int length = value.length();
    //Instantiate default flag
    bool flag = (length > 0);

    //If only a single character
    if(length == 1){
      //Check to ensure the only character is not a decimal point
      flag &= (value[0] != '.');
    }
    
    //For each character in the provided string
    for(int i = 0; flag && (i < length); i++){
      //Check for numeric and decimal point characters
      flag &= (value[i] == '.' || isdigit((int)value[i]));
    }

    return flag;
  }

  
  InputFlags::InputFlags(){

  }
  
  InputFlags::~InputFlags(){
    
  }

  void InputFlags::Add(std::string name, std::vector<std::string> flags, std::vector<int> delimiters){
    
    //Insert flags under the provided name
    this->flags[name] = flags;

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

  std::string InputFlags::Get(std::string name){
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

  double InputFlags::GetNumeric(std::string name){
    //Insantiate default variables
    double value = std::nan("1");
    Checks checker;
    
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
  
  std::vector<std::string> InputFlags::GetList(std::string name){
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

  std::vector<double> InputFlags::GetNumericList(std::string name){
    //Instantiate return values
    std::vector<double> values;
    Checks checker;
    
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

  void InputFlags::Parse(int count, char* arguments[]){

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

  bool InputFlags::Present(std::string name){
    std::vector<std::string> tempFlags = this->flags[name];

    bool status = false;
    
    for(unsigned i = 0; i < tempFlags.size(); i++){
      status |= this->present[tempFlags[i]];
    }
    
    return status;
  }
  
  std::regex InputFlags::GenerateRegularExpression(std::string flag, int type){
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

