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
      Inputs:  param (int) - The parameter to update.
               value (double) - The value to set.
      Outputs: None
      Notes:   Use the Filter::Parameter enumeration to get the appropriate value. Parameter values should not be treated as bit-wise flags. 
    */
    void SetParameter(int param, double value);
    
    /*
      Name:    SetParameter
      Purpose: Allow dynamic setting of separate flags for filtering.
      Inputs:  param (int) - The parameter to update.
               value (bool) - The value to set.
      Outputs: None
      Notes:   Use the Filter::Parameter enumeration to get the appropriate value. Parameter values should not be treated as bit-wise flags. 
    */
    void SetParameter(int param, bool value);
    
  private:
    
    void LowPass(std::vector<double> *trace);
    void HighPass(std::vector<double> *trace);
    void BandPass(std::vector<double> *trace);
    void Average(std::vector<double> *trace);
    
    //Hold the values to use in filtering operations
    std::map<int, bool> filterFlags;
    std::map<int, double> filterParameters;
  };
  
}
#endif
