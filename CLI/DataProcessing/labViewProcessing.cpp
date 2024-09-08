//Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <limits>

//HALF-IMS libraries
#include "CLIParser.h"
#include "CommaSeparatedValues.h"

//Pre-processor Variables
#define OUTPUT_FILE "output"
#define INPUT_FILE "input"
#define SAMPLE_COMPRESSION_COUNT "sample_compression"
#define SAMPLE_PER_SEGMENT "samples_per_segment"
#define CHEMICAL_COLUMN_INDEX "chemical_index"
#define LABVIEW_DATA_COLUMNS 7

//Split the provided CSV line into numeric values
std::vector<std::string> SplitLine(std::string line){
  std::vector<std::string> results;
  std::string temp;
  
  //For each character in the line
  for(unsigned i = 0, start = 0, stop = 0; i < line.length(); i++){
    //If the character is a comma or at the end of the string
    if((line[i] == ',') || ((i + 1) == line.length())){
      //Update the stop index
      stop = i;
    }

    //If the stop and start are not equal
    if(stop != start){
      //Take substring      
      temp = line.substr(start, stop-start);

      //Add substring to list
      results.push_back(temp);
      
      //Update indices to be one character beyond current
      stop += 1;
      start = stop;
    }
  }

  return results;
}

//Convert provided vect
std::vector<double> ConvertLine(std::vector<std::string> line, unsigned chemicalIndex){
  std::vector<double> results;

  //For each element in the line
  for(unsigned i = 0; i < line.size(); i++){
    //Attempt to convert element to a double
    try{
      results.push_back(std::stod(line[i]));
    }
    catch(std::invalid_argument const& ex){
      //If the current index is not the known chemical index
      if(i != chemicalIndex){
	//Print message to error stream
	std::cerr << "Exception in '" << ex.what() << "' thrown attempting to convert " << line[i] << " at position " << i << ". ";
	std::cerr << "A minimum data value has been added as a placeholder to preserve any data spacing." << std::endl;
      }
      
      //Push minimum double value
      results.push_back(std::numeric_limits<double>::min());
    }
  }

  return results;
}

int main(int argc, char *argv[]){

  //Instantiate command line input parser
  Utilities::CLIParser cli;

  //Calculate flag types to be accepted
  int flagTypeOne = Utilities::CLIParser::Dash | Utilities::CLIParser::Space;
  int flagTypeTwo = Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Space;
  int flagTypeThree = Utilities::CLIParser::Dash | Utilities::CLIParser::DoubleDash | Utilities::CLIParser::Standalone;

  //Add desired flags to parser
  cli.Add(Utilities::CLIParser::DESCRIPTION, std::vector<std::string>{""}, std::vector<int>{flagTypeThree}, "A program to process LabView output CSV file into something that can be processed by GNU Plot for HALF-IMS development. When not provided proper inputs, the program performs no action and exits.");
  cli.Add(Utilities::CLIParser::HELP, std::vector<std::string>{"h", "help"}, std::vector<int>{flagTypeThree}, "Display this help message.");
  cli.Add(Utilities::CLIParser::VERBOSE, std::vector<std::string>{"v", "verbose"}, std::vector<int>{flagTypeThree}, "Trigger verbose program output.");

  //Add user flags
  cli.Add(INPUT_FILE, std::vector<std::string>{"i", "input"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The comma-separated list of input files to process. Final line is assumed to be a blank newline character.");
  cli.Add(OUTPUT_FILE, std::vector<std::string>{"o", "output"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The output directory where to place the processed files.");
  cli.Add(SAMPLE_COMPRESSION_COUNT, std::vector<std::string>{"s", "sections"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The number of LabView sample sections to average together. Default is 4.");
  cli.Add(SAMPLE_PER_SEGMENT, std::vector<std::string>{"p", "samples-per-segment"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The number of LabView time sections to average together. Default is 250.");
  cli.Add(CHEMICAL_COLUMN_INDEX, std::vector<std::string>{"c", "chemical-column"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The column index which has the chemical name. Default is 23.");
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //Print a message and exit if the user called for the help routine
  if(cli.Help()){
    return 0;
  }

  //Declare default output as current directory
  std::string output, input;
  bool inputFlag, outputFlag, verbose;
  unsigned sampleCompressionMaximum = 4, samplesPerLabViewOutput = 250, samplesPerTimeSegment = 1000, chemicalIndex = 23;
  
  //Determine output verbosity
  verbose = cli.Present(Utilities::CLIParser::VERBOSE);
  
  //If the necessary inputs are present
  if(cli.Present(OUTPUT_FILE) && cli.Present(INPUT_FILE)){
    //Extract necessary arguments
    input = cli.Get(INPUT_FILE);
    output = cli.Get(OUTPUT_FILE);

    //If compression count maximum was provided
    if(cli.Present(SAMPLE_COMPRESSION_COUNT)){
      //Update value
      sampleCompressionMaximum = cli.GetNumeric(SAMPLE_COMPRESSION_COUNT);
    }

    //If sample per segment count was provided
    if(cli.Present(SAMPLE_PER_SEGMENT)){
      //Update value
      samplesPerLabViewOutput = cli.GetNumeric(SAMPLE_PER_SEGMENT);
    }

    //Calculate samples per time segment
    samplesPerTimeSegment = sampleCompressionMaximum * samplesPerLabViewOutput;
    
    //If the chemical column index is provided
    if(cli.Present(CHEMICAL_COLUMN_INDEX)){
      //Update value
      chemicalIndex = cli.GetNumeric(CHEMICAL_COLUMN_INDEX);
    }
    
    //Create file streams for input and output
    std::fstream inputFile, outputFile;

    //Open files
    inputFile.open(input, std::fstream::in);
    outputFile.open(output, std::fstream::out | std::fstream::trunc);

    //Determine if either file is open
    inputFlag = inputFile.is_open();
    outputFlag = outputFile.is_open();
    
    //If both file streams were opened
    if(inputFlag && outputFlag){

      //Write a header line detailing the values in each column
      outputFile << "Time (s), +V_s (V), +V_l (V), -V_s (V), -V_l (V), Detector 1 (V), Detector 2 (V), ";

      for(int i = 1; i <= 8; i++){
	outputFile << "MIPS Set Ch. " << i << " (V), ";
      }

      for(int i = 1; i <= 8; i++){
	outputFile << "MIPS Read Ch. " << i << " (V), ";
      }

      outputFile << "Chemical, Analyte Concentration (ppm), Syringe Volume (mL), Syringe Pump (mL/hr), MFC Setting (mL/min)" << std::endl;
      
      //Read the file until the LabView header line is found
      for(std::string line = "", column = ""; !inputFile.eof() && column != "X_Value"; std::getline(inputFile, line), column = line.substr(0,7)){}

      std::vector<std::string> splitLine;
      std::vector<double> avgLine, numericLine;
            
      //For each line of the input file
      double avgCount = -1;
      double timeSegmentCompressionCount = 0;
      for(std::string line = "", convertedVal="", chemical=""; !inputFile.eof(); std::getline(inputFile, line)){
	//Increment average count
	avgCount++;
	
	//Split the line into numeric values
	splitLine = SplitLine(line);
	numericLine = ConvertLine(splitLine, chemicalIndex);
	
	//Initialize average line with zeroes
	for(unsigned i = avgLine.size(); i < splitLine.size(); i++){
	  avgLine.push_back(0.0);
	}
 
	//If verbose output and line has values
	if(verbose && (splitLine.size() > 0)){
	  //Print the line being processed
	  std::cout << line << std::endl;
	}

	//If line has at least 24 elements
	if(splitLine.size() >= 24){
	  //Grab the 24th element which should be the chemical name	  
	  chemical = splitLine[23];
	}
	
	//If average count has been reached and the sample compression count has been exceeded or the end of file has been reached
	if(((avgCount == samplesPerTimeSegment) && (timeSegmentCompressionCount == sampleCompressionMaximum)) || (inputFile.eof())){
	  //Re-use line variable
	  line = "";

	  //Build a string from the line
	  for(unsigned i = 0; i < avgLine.size(); i++){
	    //If within LabView data columns
	    if(i == 0){
	      //Do nothing as this is the time value
	    }
	    else if(i < LABVIEW_DATA_COLUMNS){
	      //Use line-by-line count for averaging
	      avgLine[i] /= avgCount;
	    }
	    //If not within LabView data columns
	    else{
	      //Use time segment compression count for averaging	      
	      avgLine[i] /= timeSegmentCompressionCount;
	    }

	    //Convert value to a string and append to output line
	    if(i == 23){
	      convertedVal = chemical;
	    }
	    else{
	      convertedVal = std::to_string(trunc(avgLine[i]*100)/100);
	    }
	    line = line + convertedVal + ',';	      

	    //Reset value to zero
	    avgLine[i] = 0.0;
	  }

	  //Add newline character
	  line += '\n';
	  
	  //If verbose output and line has values
	  if(verbose){
	    //Print the processed line
	    std::cout << line << std::endl;
	  }

	  //Write line to the file
	  outputFile << line;
	  
	  //Reset average counts
	  avgCount = 0;
	  timeSegmentCompressionCount = 0;
	}
	//If not at compression maximum
	else if(splitLine.size() > LABVIEW_DATA_COLUMNS){
	  //Increase compression count
	  timeSegmentCompressionCount++;
	}

	//Add current line to the growing average
	for(unsigned i = 0; i < splitLine.size(); i++){
	  if(i == 0){
	    avgLine[i] = numericLine[i];
	  }
	  else{
	    avgLine[i] += numericLine[i];
	  }
	}
      }
      
      //Close file streams
      inputFile.close();
      outputFile.close();
    }
    //Indicate which file had trouble
    else{
      std::cout << "Unable to open ";
      
      if(!inputFlag && outputFlag){
	std::cout << "input file.";
	outputFile.close();
      }
      else if(inputFlag && !outputFlag){
	std::cout << "output file.";
	inputFile.close();
      }
      else{
	std::cout << "input and output files.";
      }

      std::cout << std::endl;
    }

    
  }
    
  return 0;
}
