# Revision by A.Gulchenko <conconky@gmail.com>
# ver 0.0.2 <05.06.2013>

#!/bin/sh
PROGRAM = conconky
OBJECTS = conconky.cpp parsers.cpp system.cpp config.cpp
HEADERS = -I./ -L/usr/lib/
COMPILER = g++
COMPILE_FLAGS = -g -Wall -std=gnu++0x
 
all:
	$(COMPILER) -o $(PROGRAM) $(COMPILE_FLAGS) $(HEADERS) $(OBJECTS)
