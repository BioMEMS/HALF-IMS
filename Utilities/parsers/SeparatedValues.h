class SeparatedValues{
public:
  //Class constructors
  SeparatedValues(string filename);
  //Class destructor
  ~SeparatedValues();

  /*
    Name:    Parse
    Purpose: Read file contents and convert to arrays of strings.
    Inputs:  None
    Outputs: None
  */
  void Parse();

  /*
    Name:    Transpose
    Purpose: Convert rows of parsed contents into columns.
    Inputs:  None
    Outputs: setting (bool) - Transpose setting.
    Notes:   Toggles a flag which causes all subsequent Parse commands to take appropriate transposing action..
  */
  bool Transpose();
  
private:
  
  /*
    Name:    SetDelimiter
    Purpose: Set the delimiting character for the class.
    Inputs:  delim (char) - The character to use for parsing files.
    Outputs: None
  */
  void SetDelimiter(char delim);

  char fileDelimiter;
  string filename;
  bool transposeFlag;
}
