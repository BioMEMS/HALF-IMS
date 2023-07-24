#ifndef SEPARATED_VALUES_H
#define SEPARATED_VALUES_H

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

class SeparatedValues{
public:

  //Class constructor
  SeparatedValues();
  SeparatedValues(std::string filename);
  
  //Class destructor
  ~SeparatedValues();
  
  /*
    Name:    ()
    Purpose: Operator overload to allow array-style matrix access.
    Inputs:  row (int) - The row number to access within the matrix.
             column (int) - The column number to access within the matrix.
    Outputs: val (std::string*) - A pointer to the string to access.
    Notes:   This function returns a pointer to allow the calling program to directly update the value similar to an array index.
             It is possible that this is a horrible idea and will need to be changed later. For now, convenience rules the day.
  */
  std::string& operator()(unsigned row, unsigned column);
  std::string operator()(unsigned row, unsigned column) const;

  /*
    Name:    Open
    Purpose: Open the file for IO operations.
    Inputs:  file (std::string) - The file to use for reading and writing operations.
    Outputs: None
    Notes:   If called after parsing is done, the object will not have the current files contents. When write is called, the new file
             will be updated with the object contents. 
  */
  void Open(std::string file);
  
  /*
    Name:    Read
    Purpose: Read file contents to allow access through object.
    Inputs:  None
    Outputs: None
  */
  void Read();

  /*
    Name:    Write
    Purpose: Write the object contents back to the file.
    Inputs:  None
    Outputs: None
    Notes:   Deletes any current contents of the file and writes all values.
  */
  void Write();
  
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

  
protected:
  void SetDelimiter(char delimiter);
  
private:
  /*
    Name:    ExpandContent
    Purpose: Expand the content matrix to ensure accessing has a known value.
    Inputs:  row (unsigned) - The row to access.
             column (unsigned) - The column to access.
    Outputs: None
    Notes:   None
   */
  void ExpandContent(unsigned row, unsigned column);
  
  std::vector<std::vector<std::string>> content;
  std::string filename;
  char fileDelimiter;
  bool readTranspose, writeTranspose;
  std::fstream file;
  unsigned maxRow, maxColumn;
};

#endif
