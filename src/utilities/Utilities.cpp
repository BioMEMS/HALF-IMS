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

double Utilities::ConvertValue_Townsends(double temperature, double pressure, double gap, double voltage){
  double efield, numberDensity;
  
  efield = voltage / gap;
  numberDensity = (pressure * PSI_TO_PASCALS) / (BOLTZMANN_CONSTANT * (temperature + 274.15));
  return EFIELD_TO_TOWNSEND * (efield / (numberDensity));
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

double Utilities::CalculateAnalyteConcentration(double carrierRate, double analyteRate, double sourceConcentration){
  //If the provided carrier rate is zero
  if(carrierRate == 0.0){
    //Return a default value of nothing
    return 0.0;
  }
  
  return sourceConcentration * (analyteRate / (60.0 * carrierRate));
}

double Utilities::CalculateSourceConcentration(double carrierRate, double analyteRate, double dilutedConcentration){
  //If the provided analyte rate is zero
  if(analyteRate == 0.0){
    //Return a default value of nothing
    return 0.0;
  }
  
  return dilutedConcentration * (60.0 * carrierRate) / analyteRate;
}

double Utilities::CalculateAnaylteRate(double carrierRate, double sourceConcentration, double analyteConcentration){
  //If analyte concentration is zero
  if(analyteConcentration == 0.0){
    //Return expected value of nothing
    return 0.0;
  }
  
  return (sourceConcentration * carrierRate * 60.0) / analyteConcentration;
}

double Utilities::CalculateCarrierRate(double sourceConcentration, double analyteConcentration, double analyteRate){
  //If analyte concentration is zero
  if(analyteConcentration == 0.0){
    //Return expected value of nothing
    return 0.0;
  }
  
  return (sourceConcentration * analyteRate) / (60.0 * analyteConcentration);
}

double Utilities::CalculateAnalyteConcentration(double analyteVolume, double analyteMolarMass, double analyteDensity, double carrierVolume, double carrierMolarMass, double carrierDensity){  
  double analyteConcentration;

  //If either carrier volume, carrier density, or analyte molar mass values are zero
  if((carrierVolume == 0.0) || (carrierDensity == 0.0) || (analyteMolarMass == 0.0)){
    //Return default value of zero
    return 0.0;
  }
  
  analyteConcentration = analyteVolume * analyteDensity * carrierMolarMass * 1E6;
  analyteConcentration /= carrierVolume * carrierDensity * analyteMolarMass;
  
  return analyteConcentration;
}

double Utilities::CalculateAnalyteVolume(double targetConcentration, double analyteMolarMass, double analyteDensity, double carrierVolume, double carrierMolarMass, double carrierDensity){
  double analyteVolume;

  //If either carrier molar mass or analyte density are zero
  if((carrierMolarMass == 0.0) || (analyteDensity == 0.0)){
    //Return default value of zero
    return 0.0;
  }
  
  //Calculate analyte volume from V = V_c * (PPM / 1E6) * (D_c * MM) / (D * MM_c)
  analyteVolume = (targetConcentration / 1000000.0);
  analyteVolume *= carrierVolume;
  analyteVolume *= (carrierDensity * analyteMolarMass) / (analyteDensity * carrierMolarMass);

  return analyteVolume;
}

double Utilities::CalculateCarrierVolume(double targetConcentration, double analyteMolarMass, double analyteDensity, double analyteVolume, double carrierMolarMass, double carrierDensity){
  double carrierVolume;

  //If either target concentration, carrier density, or analyte molar mass values are zero
  if((targetConcentration == 0.0) || (carrierDensity == 0.0) || (analyteMolarMass == 0.0)){
    //Return default value of zero
    return 0.0;
  }
  
  carrierVolume = analyteVolume * analyteDensity * carrierMolarMass;
  carrierVolume /= (targetConcentration * carrierDensity * analyteMolarMass);
  
  return carrierVolume;
}
