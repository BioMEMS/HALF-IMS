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
OUTFILE=chem-calculator.x
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

all: calculator

calculator: main.cpp $(OBJTGT)
	$(COMPILER) $(CPPFLAGS) $(SOURCE) $(OBJFILES) main.cpp -o $(OUTDIRECTORY)$(OUTFILE)

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

