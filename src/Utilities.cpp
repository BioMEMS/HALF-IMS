#include "Utilities.h"

namespace Utilities{
  InputFlags::InputFlags(){

  }
  
  InputFlags::~InputFlags(){
    
  }

  void InputFlags::Add(std::string name, std::vector<std::string> flags, std::vector<Delimiter> delimiters){
    
    //Insert flags under the provided name
    this->flags[name] = flags;

    Delimiter temp;
    //For each flag
    for(unsigned i = 0; i < flags.size(); i++){

      //If delimiters were not provided
      if(delimiters.size() == 0){
	//Default to either
	temp = Delimiter::Either;
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

  double InputFlags::Get(std::string name){
    //Insantiate default variables
    double value = nan;

    //Get the value found
    std::string temp = this->Get(name);

    //If we can convert to a numerical value
    if(Utilities::NumericalConvert(temp)){
      //Convert string to a double
      value = std::stod(temp);
    }

    //Return result
    return value;
  }
  
  std::vector<std::string> InputFlags::Get(std::string name){
    //Get the value string
    std::string temp = this->Get(name);

    //Instantiate return vector
    std::vector<std::string> values;

    //So long as positions can be found
    for(std::size_t i = 0, end = 0; i != std::string::npos; ){
      //Find a comma from the previous position
      end = temp.find(",",i);
      //Extract the substring and put into vector
      values.push_back(temp.substr(i, end-i));
    }

    return values;
  }

  std::vector<double> InputFlags::Get(std::string name){
    //Instantiate return values
    std::vector<double> values;

    //Get the list of values as strings
    std::vector<std::string> temp = this->Get(name);

    //For each string value
    for(unsigned i = 0; i < temp.size(); i++){
      //If it can be convereted
      if(Utilities::NumericalConvert(temp[i])){
	//Convert it to a double
	values.push_back(std::stod(temp[i]));
      }
      //Otherwise
      else{
	//Put a NaN in its place
	values.push_back(nan);
      }
    }

    return values;
  }

  void InputFlags::Parse(int count, char* arguments[]){

    std::string locFlag;
    std::vector<std::string> temp;
    
      //For each flag association
      for(std::map<std::string, std::vector<std::string>>::iterator iter = this->flags.begin(); iter != this->flags.end(); ++iter){
	//Get the flags associated with the key
	temp = this->flags[iter->first];
	
	//For each flag 
	for(unsigned j = 0; j < temp.size(); j++){
	  locFlag = temp[j];
	  //Get the type saved
	  Type tempType = this->types[locFlag];
	  
	  //Create a space-delimited, equal-delimited regular, and standalone expression
	  std::regex spaceDelimited = this->GenerateRegularExpression(locFlag, tempType & Type::Space);
	  std::regex equalDelimited = this->GenerateRegularExpression(locFlag, tempType & Type::Equal);
	  std::regex soloDelimited = this->GenerateRegularExpression(locFlag, tempType & Type::Standalone);
	  
	  //For each value in the arguments list
	  for(int i = 0, select = 0; i < count; i++, select = 0){
	    //If the current argument matches a standlone regular expression
	    if(std::regex_match(arguments[i], soloDelimited)){
	      
	    }
	    //If the current argument matches a space delimited regular expression 
	    else if(std::regex_match(arguments[i], spaceDelimited)){
	      if(i < (count-1)){
		this->values[locFlag] = arguments[i+1];
	      }
	      else{
		this->values[locFlag] = "";
	      }
	      this->present[locFlag] = true;
	    }
	    //If the current argument matches an equal delimited regular expression
	    else if(std::regex_match(arguments[i], equalDelimited)){
	      
	    }
	    //Otherwise
	    else{
	      //Ensure the value found is blank
	      this->values[locFlag] = "";
	      this->present[locFlag] = false;
	    }
	  }
	}
      }
      
      return;
  }

  //End of Utilities namespace
}

