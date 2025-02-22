#include <Utilities.h>

Utilities::ConvertedData Utilities::ConvertValue_Double(std::string item){
  Utilities::ConvertedData results;
  
  //Attempt to convert element to a double
  try{
    results.value = std::stod(item);
    results.error = false;
    results.msg = "";
  }
  catch(std::invalid_argument const& ex){
    results.value = 0;
    results.error = true;
    results.msg = "Exception in '" + std::string(ex.what()) + "' thrown attempting to convert '" + item + "'. A zero value has been added as a placeholder.";
  }
  
  return results;
}

double Utilities::CalculateAtoms(double current, double time){
  return (current * time) / ELEMENTARY_CHARGE;
}

double Utilities::CalculateCurrent(double atoms, double time){
  return (atoms * ELEMENTARY_CHARGE) / time;
}

double Utilities::CalculateCurrent(double voltage){
  return voltage / DETECTOR_BOARD_IDEAL_RESISTANCE;
}

double Utilities::CalculateResponse(double signal, double background){
  return 100 * (signal - background) / background;
}
