#Declare directories for source files
SOURCEDIRECTORIES=../../src/
SOURCEDIRECTORIES+=../../src/databases/
#Declare output file
OUTFILE=tests.out
#Declare output directory
OUTDIRECTORY=.

#Manipulate variables appropriately for commands
SOURCE=$(foreach DIR,$(SOURCEDIRECTORIES),-I$(DIR))

all: unit-tests

test:
	echo $(SOURCE)

unit-tests: main.cpp
	g++ -o $(OUTDIRECTORY)/$(OUTFILE) $(SOURCE) main.cpp

clean:
	rm *~ $(OUTDIRECTORY)/*.o $(OUTDIRECTORY)/$(OUTFILE)
