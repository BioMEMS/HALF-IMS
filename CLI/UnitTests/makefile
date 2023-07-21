#Declare directories for source files
SOURCEDIRECTORIES=../../src/
SOURCEDIRECTORIES+=../../src/databases/
#Declare output file
OUTFILE=tests.out

#Manipulate variables appropriately for commands
SOURCE=$(foreach DIR,$(SOURCEDIRECTORIES),-I$(DIR))

all: unit-tests

test:
	echo $(SOURCE)

unit-tests: main.cpp
	g++ -o $(OUTFILE) $(SOURCE) main.cpp
