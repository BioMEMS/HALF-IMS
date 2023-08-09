#include "Filter.h"

namespace SignalProcessing{

  //Public functions
  Filter::Filter(){
    
  }

  Filter::~Filter(){
    
  }
  
  std::vector<double> Filter::Apply(std::vector<double> trace, Operation filter){
    std::vector<double> output;
    
    if(filter == Operation::LowPassFilter){
      output =  LowPass(trace);
    }
    else if(filter == Operation::HighPassFilter){
      output = HighPass(trace);
    }
    else if(filter == Operation::BandPassFilter){
      output = BandPass(trace);
    }
    else if(filter == Operation::WeightedAverage){
      output = Average(trace);
    }
    
    return output;
  }

  void Filter::SetParameter(Parameters param, double value){
    return;
  }
  
  void Filter::SetParameter(Parameters param, bool value){
    return;
  }
  
  //Private functions
  std::vector<double> Filter::LowPass(std::vector<double> trace){
    return trace;
  }

  std::vector<double> Filter::HighPass(std::vector<double> trace){
    return trace;
  }

  std::vector<double> Filter::BandPass(std::vector<double> trace){
    return trace;
  }

  std::vector<double> Filter::Average(std::vector<double> trace){
    //Get the aperture size defined
    int aperture = 20, traceSize = trace.size();

    //Instantiate the output trace with input size
    std::vector<double> output;
    output.resize(traceSize);
    
    //Instantiate filter weight value
    double weight[aperture];
    double average = 0, weightSum = 0;
    
    //Pre-calculate all weight factors
    for(int i = 0, target = aperture/2; i < aperture; i++){
      //Calculate the weight as the distance from the target subtracted from the aperture size
      weight[i] = aperture - std::abs(i - target);
      
      //Generate the weight summation value
      weightSum += weight[i];
    }
    
    //For every point in the trace
    for(int i = 0, start = 0, stop = 0; i < traceSize; i++){
      
      //Calculate the ideal start and stop points
      start = i - aperture/2;
      stop = start + aperture;

      //If the start is below zero
      if(start < 0){
	//Set to zero
	start = 0;
      }

      //If the stop index is off the trace
      if(stop >= traceSize){
	//Set to the last point
	stop = traceSize - 1;
      }

      //Clear average value
      average = 0;
      
      //For every point within the aperture
      for(int j = start, k = 0; j <= stop; j++, k++){
	//Sum the trace value multiplied by the weight
	average += (weight[k]*trace[j]);
      }

      //Calculate the weighted average and place into output trace
      output[i] = average/weightSum;
    }

    //Return calculated values
    return output;
  }

}
