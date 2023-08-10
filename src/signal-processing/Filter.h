#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <map>
#include <string>

namespace SignalProcessing{

  class Filter{
  public:
    Filter();
    ~Filter();
    
    enum Operation{
      LowPassFilter,
      HighPassFilter,
      BandPassFilter,
      WeightedAverage
    };

    enum Parameters{
      Aperture=0,
    };
    
    /*
      Name:    Apply
      Purpose: Apply the desired filter type to the trace.
      Inputs:  trace (std::vector<double>) - The entire data trace to which the filter will be applied.
               filter (FilterType) - The filter type enumeration to dynamically select.
      Output:  updated (std::vector<double>) - The trace data with the filter applied.
      Notes:   There are four filter types: LowPass, HighPass, BandPass, and Average. Average filtering
      applies a weighted average to the signal.
    */
    void Apply(std::vector<double> *trace, Operation filter);
    
    /*
      Name:    SetParameter
      Purpose: Allow dynamic setting of separate values for filtering.
      Inputs:  param (std::string) - The parameter name to update.
               value (double) - The value to set.
      Outputs: None
      Notes:   None
    */
    void SetParameter(Parameters param, double value);
    
    /*
      Name:    SetParameter
      Purpose: Allow dynamic setting of separate flags for filtering.
      Inputs:  param (std::string) - The parameter name to update.
               value (bool) - The value to set.
      Outputs: None
      Notes:   None
    */
    void SetParameter(Parameters param, bool value);
    
  private:
    
    void LowPass(std::vector<double> *trace);
    void HighPass(std::vector<double> *trace);
    void BandPass(std::vector<double> *trace);
    void Average(std::vector<double> *trace);
    
    //Hold the values to use in filtering operations
    std::map<std::string, bool> filterFlags;
    std::map<std::string, double> filterParamters;
  };
  
}
#endif
