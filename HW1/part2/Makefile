#
# Makefile framework for CptS223
#
# Hacked up by Aaron Crandall, 2017
# Variables
GPP     = gcc
CFLAGS  = -m32 -g -Wall
RM      = rm -f
BINNAME = a.out

# Shell gives make a full user environment
# Adding this to PATH will find the newer g++ compiler on the EECS servers.
SHELL := /bin/bash
PATH := /opt/rh/devtoolset-3/root/usr/bin/:$(PATH)


# Default is what happenes when you call make with no options
#  In this case, it requires that 'all' is completed
default: all

# All is the normal default for most Makefiles
#  In this case, it requires that build is completed
all: build

# build depends upon *.cpp, then runs the command:
#  g++ -g -std=c++0x -o bigFiveList
build: part2.c
	$(GPP) $(CFLAGS) -o $(BINNAME) part2.c

run: build
	./$(BINNAME)

# If you call "make clean" it will remove the built program
clean veryclean:
	$(RM) $(BINNAME)
