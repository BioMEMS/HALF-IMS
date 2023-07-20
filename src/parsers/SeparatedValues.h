#ifndef SEPARATED_VALUES_H
#define SEPARATED_VALUES_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

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
    Inputs:  read (bool) - Flag which determines if all indexing operations should consider rows to be columns and columns to be rows.
             write (bool) - Flag which determines if contents written to the file will be transposed.
    Outputs: None
    Notes:   Allows the setting of two flags. The first flag determines if a read operation will be considered to be "transposed" for indexing.
    If set, this means that the first index value will be considered a column from the original file while the second index value will be considered a row.
    If not set, the first index value will be considered a row from the original file while the second index value will be considered a column.
    The second flag determines if the matrix read from the file should be transposed when writing back to the file. This will not affect anything
    other than the file written to disk. All indexing operations will following the first flag.
  */
  void Transpose(bool read, bool write);
  
private:

  std::vector<std::vector<string>> content;
  char fileDelimiter;
  string filename;
  bool readTranspose, writeTranspose;
  std::ofstream file;
}

#endif
