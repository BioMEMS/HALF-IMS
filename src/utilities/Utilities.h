#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <iostream>

#define ELEMENTARY_CHARGE 1.6E-19
#define AMU_TO_GRAMS 1.660539E-24

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
}

#endif
