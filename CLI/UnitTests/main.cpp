//Standard libraries
#include <iostream>
#include <fstream>

//HALF-IMS libraries
#include "Utilities.h"
#include "SeparatedValues.h"
#include "CommaSeparatedValues.h"

//Global variables for test development
std::vector<std::string> contents = {"one", "two", "three", "four", "five", "six"};
unsigned rows = 2;
unsigned columns = 3;

namespace UnitTestsSetup{
    //Simple function to convert boolean to string with some flavor
    std::string StatusOutput(bool status){
      if(status){
	return "\033[92mPASS\033[0m";
      }
      else{
	return "\033[31mFAIL\033[0m";
      }
    }

  //Simple function to standardize printing ouputs
  void PrintLine(std::string filename, std::string name, bool status){
    std::cout << filename <<  " Unit Tests (" << name << "): " << StatusOutput(status) << std::endl;
    
    return;
  }

  
  void WriteBasicSetupFile(std::string file, char delimiter){
    //Open the temporary file for output and truncate the file
    std::fstream tempFile;
    tempFile.open(file, std::fstream::out | std::fstream::trunc);

    //Write the contents
    for(unsigned i = 0; i < rows; i++){
      for(unsigned j = 0; j < columns; j++){
	tempFile << contents[i*columns + j] << delimiter;
      }
      tempFile << std::endl;
    }
    
    //Close the file
    tempFile.close();

    return;
  }

  void WriteQuotedSetupFile(std::string file, char delimiter){
    std::fstream tempFile;

    //Open the temporary file for output and truncate the file
    tempFile.open(file, std::fstream::out | std::fstream::trunc);

    //Write the contents
    for(unsigned i = 0; i < rows; i++){
      for(unsigned j = 0; j < columns; j++){
	tempFile << '"' << contents[i*columns + j] << '"' << delimiter;
      }
      tempFile << std::endl;
    }

    //Close the file
    tempFile.close();

    return;
  }
  
  void WritePartQuotedSetupFile(std::string file, char delimiter){
    std::fstream tempFile;
    
    //Open the temporary file for output and truncate the file
    tempFile.open(file, std::fstream::out | std::fstream::trunc);
    
    //Write the contents
    for(unsigned i = 0; i < rows; i++){
      for(unsigned j = 0; j < columns; j++){
	if((i == 0 && (j == 0 || j == 2)) || (i == 1 && j == 1)){
	  tempFile << '"' << contents[i*columns + j] << '"' << delimiter;
	}
	else{
	  tempFile << contents[i*columns + j] << delimiter;
	}
      }
      tempFile << std::endl;
    }
    
    //Close the file
    tempFile.close();
    
    return;
  }
  
}

namespace UtilitiesUnitTests{
  bool BasicUnit(){
    bool status = true;
    Utilities::Checks temp;

    status = temp.NumericalConvert("1.0");
    
    return status;
  }

  bool PeriodCheck(){
    bool status = true;
    Utilities::Checks temp;

    return !temp.NumericalConvert(".");
  }
}

namespace CSVUnitTests{
  bool BasicUnit(){
    bool status = true;

    CommaSeparatedValues temp("temp.csv");
    
    return status;
  }

  bool CheckContent(){

    bool status = false;
    std::string file = "temp.csv";
    UnitTestsSetup::WriteQuotedSetupFile(file, ',');

    try{
      status = true;
      CommaSeparatedValues temp(file);
      temp.Read();

      for(unsigned i = 0; i < rows; i++){
	for(unsigned j = 0; j < columns; j++){
	  status &= (contents[i*columns + j].compare(temp(i,j)) == 0);
	}
      }
    }
    catch(const std::exception& exception){
      status = false;
    }

    return status;
  }
}
  
namespace SVUnitTests{
  //All the unit tests for the database classes
  bool BasicUnit(){
    bool status = true;

    SeparatedValues temp("temp.sv");

    return status;
  }

  bool ReadFileUnit(){
    bool status = false;

    std::string file = "temp.sv";

    UnitTestsSetup::WriteBasicSetupFile(file, ' ');
    
    try{
    SeparatedValues temp(file);
    temp.Read();
    status = true;
    }
    catch(const std::exception& exception){
      //Do nothing
    }

    return status;
  }

  bool ReadQuotedFileUnit(){
    bool status = false;

    std::string file = "temp.sv";
    
    UnitTestsSetup::WriteQuotedSetupFile(file, ' ');
    
    try{
    SeparatedValues temp(file);
    temp.Read();
    status = true;
    }
    catch(const std::exception& exception){
      //Do nothing
    }

    return status;
  }

  bool ReadPartQuotedFileUnit(){
    bool status = false;

    std::string file = "temp.sv";

    UnitTestsSetup::WritePartQuotedSetupFile(file, ' ');

    try{
    SeparatedValues temp(file);
    temp.Read();
    status = true;
    }
    catch(const std::exception& exception){
      //Do nothing
    }

    return status;
  }

  bool ReadBadFileUnit(){
    bool status = false;

    try{
      SeparatedValues temp("temp.sv");
      temp.Read();
      status = true;
    }
    catch(const std::exception& exception){
      //Do nothing
    }

    return status;
  }

  bool CheckContents(){
    bool status = true;

    std::string file = "temp.sv";

    for(int v = 0; v < 3; v++){

      //Write different setup files based on variant
      if(v == 0){
	UnitTestsSetup::WriteBasicSetupFile(file, ' ');
      }
      else if(v == 1){
	UnitTestsSetup::WriteQuotedSetupFile(file, ' ');
      }
      else{
        UnitTestsSetup::WritePartQuotedSetupFile(file, ' ');
      }
      
      try{
	SeparatedValues temp(file);
	temp.Read();
	
	for(unsigned i = 0; i < rows; i++){
	  for(unsigned j = 0; j < columns; j++){
	    status &= (contents[i*columns + j].compare(temp(i,j)) == 0);
	  }
	}
	
	temp.Transpose(true, true);
	
	for(unsigned i = 0; i < rows; i++){
	  for(unsigned j = 0; j < columns; j++){
	    status &= (contents[i*columns + j].compare(temp(j,i)) == 0);
	  }
	}
      }
      catch(const std::exception& exception){
	status = false;
      }
    }
    return status;
  }

  bool WriteContent(){
    bool status = false;

    std::string file = "temp.sv";
    std::string updateFile = "update.sv";
    UnitTestsSetup::WriteBasicSetupFile(file, ' ');

    try{
      SeparatedValues temp(file);
      temp.Read();
      
      temp(0,0) = "blank";

      temp.Open(updateFile);
      temp.Write();

      SeparatedValues tempUpdate(updateFile);
      tempUpdate.Read();
      
      status = (temp(0,0).compare(tempUpdate(0,0)) == 0);
      
    }
    catch(const std::exception& exception){
      std::cout << exception.what() << std::endl;
      status = false;
    }
    
    
    return status;
  }

}


int main(int argc, char *argv[]){
  
  UnitTestsSetup::PrintLine("SeparatedValues.cpp", "Blank Constructor", SVUnitTests::BasicUnit());
  UnitTestsSetup::PrintLine("SeparatedValues.cpp", "File Parse Crash", SVUnitTests::ReadFileUnit());
  UnitTestsSetup::PrintLine("SeparatedValues.cpp", "Non-existent File Parse Crash", SVUnitTests::ReadBadFileUnit());
  UnitTestsSetup::PrintLine("SeparatedValues.cpp", "Contents Check", SVUnitTests::CheckContents());
  UnitTestsSetup::PrintLine("SeparatedValues.cpp", "Quoted Contents Check", SVUnitTests::ReadQuotedFileUnit());
  UnitTestsSetup::PrintLine("SeparatedValues.cpp", "Part Quoted Contents Check", SVUnitTests::ReadPartQuotedFileUnit());
  UnitTestsSetup::PrintLine("SeparatedValues.cpp", "Write Contents Check", SVUnitTests::WriteContent());
  UnitTestsSetup::PrintLine("CommaSeparatedValues.cpp", "Blank Constructor", CSVUnitTests::BasicUnit());
  UnitTestsSetup::PrintLine("CommaSeparatedValues.cpp", "Contents Check", CSVUnitTests::CheckContent());
  UnitTestsSetup::PrintLine("Utilities.cpp", "Numeric Check", UtilitiesUnitTests::BasicUnit());
  UnitTestsSetup::PrintLine("Utilities.cpp", "Period Check", UtilitiesUnitTests::PeriodCheck());
  
  return 0;
}
