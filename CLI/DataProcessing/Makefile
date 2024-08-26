#Declare compiler
COMPILER=g++
#Declare compiler flags
CPPFLAGS=-ggdb -std=c++17 -Wall -Wextra
#Declare directories for source files
SOURCEDIRECTORIES=../../src/
SOURCEDIRECTORIES+=../../src/databases/
SOURCEDIRECTORIES+=../../src/utilities/
SOURCEDIRECTORIES+=../../src/signal-processing/
#Declare output file
OUTFILE=processing.x
#Declare output directory
OUTDIRECTORY=bin/

#Get all source and header files in the source directories
SRCFILES=$(foreach DIR,$(SOURCEDIRECTORIES),$(wildcard $(DIR)*.cpp))
HDRFILES=$(foreach DIR,$(SOURCEDIRECTORIES),$(wildcard $(DIR)*.h))
SOURCE=$(foreach DIR,$(SOURCEDIRECTORIES),-I$(DIR))
#Convert source files into object file targets
OBJTGT=$(SRCFILES:.cpp=.o)
#Convert object file targets into object file outputs
OBJFILES:=$(foreach FILE,$(OBJTGT),$(OUTDIRECTORY)$(notdir $(FILE)))

all: 
	make peak-processing OUTFILE=peak-processing.x
	make labview-processing OUTFILE=labview-processing.x

peak-processing: peakProcessing.cpp $(OBJTGT)
	$(COMPILER) $(CPPFLAGS) $(SOURCE) $(OBJFILES) peakProcessing.cpp -o $(OUTDIRECTORY)$(OUTFILE)

labview-processing: labViewProcessing.cpp $(OBJTGT)
	$(COMPILER) $(CPPFLAGS) $(SOURCE) $(OBJFILES) labViewProcessing.cpp -o $(OUTDIRECTORY)$(OUTFILE)

%.o: %.cpp %.h
	@mkdir -p $(OUTDIRECTORY)
	$(COMPILER) $(CPPFLAGS) $(SOURCE) -c $< -o $(OUTDIRECTORY)$(notdir $@) 

.PHONY: clean
clean:
	@rm *~ $(OUTDIRECTORY)*

.PHONY: test
test:
	@printf "%s\n" "Source Directory" $(SOURCE)
	@printf "%s\n" "Header Files" $(HDRFILES)
	@printf "%s\n" "Source Files" $(SRCFILES)
	@printf "%s\n" "Object Targets" $(OBJTGT)
	@printf "%s\n" "Object Files" $(OBJFILES)

