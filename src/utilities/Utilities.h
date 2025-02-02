#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <iostream>
#include <vector>

#define ELEMENTARY_CHARGE 1.6E-19
#define AMU_TO_GRAMS 1.660539E-24
#define DETECTOR_BOARD_IDEAL_RESISTANCE 300E9

namespace Utilities{
  //Simple limits structure to hold values
  struct Limits{
    unsigned Rows;
    unsigned Columns;
    double MinimumValue;
    double MaximumValue;
  };

  struct ConvertedData{
    double value;
    bool error;
    std::string msg;
  };
  
  //Convert provided value
  ConvertedData ConvertValue_Double(std::string item);

  //Convert atomic mass units (AMU) to grams
  ConvertedData ConvertValue_MassUnits(double mass);

  //Convert grams to atomic mass units (AMU)
  ConvertedData ConvertValue_MassGrams(double mass);

  //Convert current to number of atoms per Faraday Cup equations
  double CalculateAtoms(double current, double time);
			
  //Convert number of atoms to current per Faraday Cup equations
  double CalculateCurrent(double atoms, double time);

  //Convert the detector voltage value to an ideal current per Detector Board equations
  double CalculateCurrent(double voltage);

  //Check presence of item in list
  template <typename T> bool ContainsItem(std::vector<T> list, T item){
    
    bool present = false;
    
    for(unsigned i = 0; !present && (i < list.size()); i++){
      present |= (list[i] == item);
    }
    
    return present;
  }
  
}

#endif
