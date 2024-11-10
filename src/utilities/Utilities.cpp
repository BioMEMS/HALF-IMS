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

