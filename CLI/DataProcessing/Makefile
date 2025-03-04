#Declare compiler
COMPILER=g++
#Declare compiler flags
CPPFLAGS=-ggdb -std=c++26 -Wall -Wextra
#Declare linker flags
LDFLAGS=$(CPPFLAGS)
LDFLAGS+=-lutil -lboost_iostreams -lboost_system -lboost_filesystem
#Declare output file
OUTFILE=processing.x

#Declare output directory
OUTDIRECTORY=bin/

#Declare directories for source files
SOURCEDIRECTORIES=../../src/
SOURCEDIRECTORIES+=../../src/databases/
SOURCEDIRECTORIES+=../../src/utilities/
SOURCEDIRECTORIES+=../../src/signal-processing/
SOURCEDIRECTORIES+=../../dependencies/gnuplot-iostream/
SOURCEDIRECTORIES+=$(OUTDIRECTORY)
VPATH:=$(SOURCEDIRECTORIES)

#Get all source and header files in the source directories
SRCFILES=$(foreach DIR,$(SOURCEDIRECTORIES),$(wildcard $(DIR)*.cpp))
HDRFILES=$(foreach DIR,$(SOURCEDIRECTORIES),$(wildcard $(DIR)*.h))
SOURCE=$(foreach DIR,$(SOURCEDIRECTORIES),-I$(DIR))
#Convert source files into object file targets
OBJTGT=$(SRCFILES:.cpp=.o)
#Convert object file targets into object file outputs
OBJFILES:=$(foreach FILE,$(OBJTGT),$(OUTDIRECTORY)$(notdir $(FILE)))

all: 
	make --no-print-directory $(OUTDIRECTORY)data-processing.x OUTFILE=data-processing.x
	make --no-print-directory $(OUTDIRECTORY)labview-processing.x OUTFILE=labview-processing.x
	make --no-print-directory $(OUTDIRECTORY)simion-processing.x OUTFILE=simion-processing.x

$(OUTDIRECTORY)data-processing.x: dataProcessing.cpp $(OBJFILES)
	make --no-print-directory output-directory
	$(COMPILER) $(LDFLAGS) $(SOURCE) $(OBJFILES) $< -o $@ 

$(OUTDIRECTORY)labview-processing.x: labViewProcessing.cpp $(OBJFILES)
	make --no-print-directory output-directory
	$(COMPILER) $(CPPFLAGS) $(SOURCE) $(OBJFILES) $< -o $@

$(OUTDIRECTORY)simion-processing.x: simionProcessing.cpp $(OBJFILES)
	make --no-print-directory output-directory
	$(COMPILER) $(CPPFLAGS) $(SOURCE) $(OBJFILES) $< -o $@

$(OUTDIRECTORY)%.o: %.cpp %.h
	$(COMPILER) $(CPPFLAGS) $(SOURCE) -c $< -o $@

.PHONY: output-directory
output-directory:
	@mkdir -p $(OUTDIRECTORY)

.PHONY: clean
clean:
	rm -r *~ $(OUTDIRECTORY)*
