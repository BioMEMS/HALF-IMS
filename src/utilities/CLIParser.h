#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <map>
#include <cmath>
#include <regex>
#include <iostream>

#include "Checker.h"

namespace Utilities{

  class CLIParser{
  public:
    //Class constructor
    CLIParser();
    //Class Destructor
    ~CLIParser();

    //Enumeration denoting accepted delimiter type
    enum Types{
      Space = 1,
      Equal = 2,
      Standalone = 4,
      Dash = 8,
      DoubleDash = 16
    };

    //Constant strings to be used during additions for help message displays
    static const std::string description;
    static const std::string help;
    
    /*
      Name:    Add
      Purpose: Establish a relationship between a human-readable name, flag, and accepted delimiters.
      Inputs:  name (std::string) - The human-readable name to use as reference.
               flags (std::vector<std::string>) - The command-line input flags to associate with the human-readable name.
	       delimiters (std::vector<int>) - The delimiter enumeration values to associate with each flag.
	       description (std::string) - The contents to display during a help call.
      Outputs: None
      Notes:   Each flag provided can have different delimiter selections denoted by enumeration value. This will affect the values parsed from a provided array.
               The flags provided have a one-to-one relationship with the delimiters provided. If a flag does not have an equivalent delimiter, the last delimiter in the
	       provided list will be used. 
    */
    void Add(std::string name, std::vector<std::string> flags, std::vector<int> delimiters, std::string description);

    /*
      Name:    Get
      Purpose: Get the desired value from the command-line inputs.
      Inputs:  name (std::string) - The name of the value to retrieve.
      Outputs: value (std::string) - The value parsed from the command-line inputs.
      Notes:   None
    */
    std::string Get(std::string name);

    /*
      Name:    GetList
      Purpose: Get the desired value from the command-line inputs.
      Inputs:  name (std::string) - The name of the value to retrieve.
      Outputs: values (std::vector<std::string>) - The values parsed from the command-line inputs.
      Notes:   None
    */
    std::vector<std::string> GetList(std::string name);
    
    /*
      Name:    GetNumeric
      Purpose: Get the desired value from the command-line inputs.
      Inputs:  name (std::string) - The name of the value to retrieve.
      Outputs: value (double) - The value parsed from the command-line inputs.
      Notes:   Will return NaN if unable to convert value.
    */
    double GetNumeric(std::string name);

    /*
      Name:    GetNumericList
      Purpose: Get the desired value from the command-line inputs.
      Inputs:  name (std::string) - The name of the value to retrieve.
      Outputs: value (std::vector<double>) - The values parsed from the command-line inputs.
      Notes:   Will put NaN if unable to convert a value.
    */
    std::vector<double> GetNumericList(std::string name);
   
    /*
      Name:    Parse
      Purpose: Find all flags and their respective outputs.
      Inputs:  count (int) - The number of arguments provided.
               arguments (char*[]) - A pointer to an array of C-style strings. Typically taken directly from the inputs to the main function.
      Outputs: None
      Notes:   None
    */
    void Parse(int count, char* arguments[]);

    /*
      Name:    Present
      Purpose: Check if a flag was found in the command-line inputs.
      Inputs:  name (std::string) - The name of the value to retrieve.
      Outputs: found (bool) - Flag which indicates if the flag was found.
      Notes:   None
    */
    bool Present(std::string name);

    /*
      Name:    Help
      Purpose: Print a help message to standard console.
      Inputs:  None
      Outputs: exit (bool) - A flag indicating if the help routine was printed.
      Notes:   This should later be expanded to allow for sending values to an arbitrary file stream.
               If the name string provided is the publicly available "description" value from this class, the description
	       will be printed out without the flags to allow a program summary.
    */
    bool Help();

  private:
    /*
      Name:    GenerateRegularExpression
      Purpose: Create a regular expression object that obeys command-line input convention.
      Inputs:  flag (std::string) - The flag to use in the expression.
               type (Types) - The bit-field that indicates how to generate the expression.
      Outputs: expression (std::regex) - The generated regular expression.
      Note:    The generated expression cannot be any combination of space-delimited, equal-delimited, or standalone.
               Standalone will be selected as default if provided a mix of these conditions. 
    */
    std::regex GenerateRegularExpression(std::string flag, int type);
    
    //Referential array to track the human-readable name to CLI flags
    std::map<std::string, std::vector<std::string>> flags;
    //Referential array to track the description strings provided for each CLI flag.
    std::map<std::string, std::string> descriptions;
    //Referential array to track the flag to CLI delimiter type
    std::map<std::string, int> types;
    //Referential array to track the flag to found value
    std::map<std::string, std::string> values;
    //Referential array to track if the flag is present
    std::map<std::string, bool> present;
  };
}
#endif
