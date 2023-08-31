#include "Filter.h"
#include <iostream>

namespace SignalProcessing{

  //Public functions
  Filter::Filter(){
    SetParameter(Parameters::Aperture, 20.0);
  }

  Filter::~Filter(){
    
  }
  
  void Filter::Apply(std::vector<double> *trace, Operation filter){
    
    if(filter == Operation::LowPassFilter){
      LowPass(trace);
    }
    else if(filter == Operation::HighPassFilter){
      HighPass(trace);
    }
    else if(filter == Operation::BandPassFilter){
      BandPass(trace);
    }
    else if(filter == Operation::WeightedAverage){
      Average(trace);
    }
    
    return;
  }

  void Filter::SetParameter(int param, double value){
    //If the parameter exists within the mapping
    if(filterParameters.count(param)){
      //Overwrite the value
      filterParameters[param] = value;
    }
    //Otherwise
    else{
      //Insert the key-value pair
      filterParameters.insert({param, value});
    }
      
    return;
  }
  
  void Filter::SetParameter(int param, bool value){
    //If the parameter exists within the mapping
    if(filterFlags.count(param)){
      //Overwrite the value
      filterFlags[param] = value;
    }
    //Otherwise
    else{
      //Insert the key-value pair
      filterFlags.insert({param, value});
    } 
    return;
  }
  
  //Private functions
  void Filter::LowPass(std::vector<double> *trace){
    for(unsigned i = 0; i < (*trace).size(); i++){
      //Do nothing
    }
    return;
  }

  void Filter::HighPass(std::vector<double> *trace){
    for(unsigned i = 0; i < (*trace).size(); i++){
      //Do nothing
    }
    return;
  }

  void Filter::BandPass(std::vector<double> *trace){
    for(unsigned i = 0; i < (*trace).size(); i++){
      //Do nothing
    }
    return;
  }

  void Filter::Average(std::vector<double> *trace){
    //Get the aperture size defined
    int aperture = filterParameters[Parameters::Aperture];

    //Save the trace length
    int traceSize = (*trace).size();

    //If the aperture exceeds the trace length
    if(aperture > traceSize){
      //Do nothing
      return;
    }
    
    //Instantiate the output trace with input size
    unsigned calculationBufferSize = aperture/2 + 1;
    //Indices to track from where the buffer should be read and written
    unsigned bufferWrite = 0, bufferRead = 0;
    //Allocate buffer on the heap which will be treated like a circular array
    double* output = new double[calculationBufferSize];
    
    //Instantiate filter weight value
    double* weight = new double[aperture];
    double average = 0, weightSum = 0;
    
    //Pre-calculate all weight factors
    for(int i = 0, target = aperture/2; i < aperture; i++){
      //Calculate the weight as the distance from the target subtracted from the aperture size
      weight[i] = aperture - std::abs(i - target);
      
      //Generate the weight summation value
      weightSum += weight[i];
    }
    
    //For every point in the trace
    for(int i = 0, start = 0, stop = 0; i < traceSize; i++, bufferWrite++){
      
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
      for(int j = start, k = 0; j <= stop; j++, k++){ //Need to calculate K index based upon difference between start and stop relative to total weights
	//Sum the trace value multiplied by the weight
	average += (weight[k]*(*trace)[j]);
      }

      //If the write index exceeds the buffer size
      if(bufferWrite >= calculationBufferSize){
	//Place read index value into the trace
	(*trace)[i - calculationBufferSize] = output[bufferRead];
	
	//If the read index is at or off the array
	if(bufferRead <= calculationBufferSize){
	  //Increment buffer read index
	  bufferRead++;
	}
	//Otherwise
	else{
	  //Reset read index
	  bufferRead = 0;
	}
	
	//Reset buffer write
	bufferWrite=0;

      }
      //Calculate the weighted average and place into output trace
      output[bufferWrite] = average/weightSum;
    }

    //Update provided array with new values
    for(unsigned i = 0; i < calculationBufferSize; i++){
      (*trace)[i] = output[i];
    }

    //Clean up allocated memory
    delete[] weight;
    delete[] output;
    
    return;
  }

}
