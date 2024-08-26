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
#define LABVIEW_DATA_COLUMNS 7

//Split the provided CSV line into numeric values
std::vector<double> SplitLine(std::string line){
  std::vector<double> results;
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
      temp = line.substr(start, stop-start);
      //Take substring
      try{
	results.push_back(std::stod(temp));
      }
      catch(std::invalid_argument const& ex){
	std::cerr << "Exception in '" << ex.what() << "' thrown attempting to grab characters " << start << " through " << stop << " from:"<< std::endl;
	std::cerr << line << std::endl;
	std::cerr << "A minimum data value has been added as a placeholder to preserve any data spacing." << std::endl;

	//Push minimum double value
	results.push_back(std::numeric_limits<double>::min());
      }
      
      //Update indices to be one character beyond current
      stop += 1;
      start = stop;
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
  cli.Add(SAMPLE_COMPRESSION_COUNT, std::vector<std::string>{"s", "samples"}, std::vector<int>{flagTypeOne, flagTypeTwo}, "The number of LabView sample sections to average together. Default is 4.");
  
  //Parse provided arguments list
  cli.Parse(argc, argv);

  //Print a message and exit if the user called for the help routine
  if(cli.Help()){
    return 0;
  }

  //Declare default output as current directory
  std::string output, input;
  bool inputFlag, outputFlag, verbose;
  double sampleCompressionMaximum = 4;
  
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

      outputFile << "MFC Setting (mL/min), Syringe Pump (mL/hr), Syringe Volume (mL), Analyte Concentration (ppm)" << std::endl;
      
      //Read the file until the LabView header line is found
      for(std::string line = "", column = ""; !inputFile.eof() && column != "X_Value"; std::getline(inputFile, line), column = line.substr(0,7)){}
      
      std::vector<double> avgLine, splitLine;
            
      //For each line of the input file
      double avgCount = 0;
      double timeSegmentCompressionCount = 0;
      for(std::string line = ""; !inputFile.eof(); std::getline(inputFile, line)){
	//Split the line into numeric values
	splitLine = SplitLine(line);

	//Initialize average line with zeroes
	for(unsigned i = avgLine.size(); i < splitLine.size(); i++){
	  avgLine.push_back(0.0);
	}
 
	//If verbose output and line has values
	if(verbose && (splitLine.size() > 0)){
	  //Print the line being processed
	  std::cout << line << std::endl;
	}
	
	//If the current line has more than LabView data columns and the sample compression count has been exceeded or the end of file has been reached
	if(((splitLine.size() > LABVIEW_DATA_COLUMNS) && (timeSegmentCompressionCount >= sampleCompressionMaximum)) || (inputFile.eof())){
	  
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
	    line = line + std::to_string(trunc(avgLine[i]*100)/100) + ',';	      

	    //Reset value to zero
	    avgLine[i] = 0.0;
	  }

	  //Add newline character
	  line += '\n';
	  
	  //If verbose output and line has values
	  if(verbose){
	    //Print the line being processed
	    std::cout << line;
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
	    avgLine[i] = splitLine[i];
	  }
	  else{
	    avgLine[i] += splitLine[i];
	  }
	}

	//Increment average count
	avgCount++;
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
