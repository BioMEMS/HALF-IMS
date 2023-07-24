#Declare compiler
COMPILER=g++
#Declare compiler flags
CPPFLAGS=-ggdb -std=c++17 -Wall -Wextra
#Declare directories for source files
SOURCEDIRECTORIES=../../src/
SOURCEDIRECTORIES+=../../src/databases/
#Declare output file
OUTFILE=tests.out
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

all: unit-tests
	./$(OUTDIRECTORY)$(OUTFILE)

unit-tests: main.cpp $(OBJTGT)
	$(COMPILER) $(CPPFLAGS) $(SOURCE) $(OBJFILES) main.cpp -o $(OUTDIRECTORY)$(OUTFILE)

%.o: %.cpp %.h
	@mkdir -p $(OUTDIRECTORY)
	$(COMPILER) $(CPPFLAGS) $(SOURCE) -c $< -o $(OUTDIRECTORY)$(notdir $@) 

.PHONY: clean
clean:
	@rm *~ *.*sv $(OUTDIRECTORY)*.o $(OUTDIRECTORY)$(OUTFILE)

.PHONY: debug
debug:
	@printf "%s\n" $(SOURCE) $(HDRFILES) $(SRCFILES) $(OBJTGT) $(OBJFILES)
