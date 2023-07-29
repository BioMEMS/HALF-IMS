#ifndef FILTER_H
#define FILTER_H

#include <map>

class Filter{
 public:
  Filter();
  ~Filter();

  /*
    Name:    Process
    Purpose: Provide a standardized way of accessing filtering methods.
    Inputs:  trace (std::vector<double>) - The values to filter with the previously selected method.
    Outputs: filtered (std::vector<double>) - The trace with filtering applied.
    
  */
  std::vector<double> Process(std::vector<double> trace);

  /*
    Name:    ()
    Purpose: Allow dynamic assignment of named factors for filtering functions.
    Inputs:  name (std::string) - The name of the numerical property to update.
             value (std::string) - The new value to use.
    Outputs: success (bool) - Flag indicating if the property was successfully updated.
    Notes:   This allows for easier setting of the class properties without having to generate
             a setter for each one or expose private member variables. Depending on the property,
	     the provided "value" will be converted to numerical or boolean types. 
  */
  bool operator()(std::string name, std::string value);
  
 private:
  
  std::vector<double> Smooth(std::vector<double> trace);
  std::vector<double> LowPass(std::vector<double> trace);
  std::vector<double> HighPass(std::vector<double> trace);

  //Private variables
  std::map<std::string, double> factors;
  std::map<std::string, std::string> functions;
  std::map<std::string, bool> flags;
}

#endif
