GPP     = gcc
CFLAGS  = -m32 -g -Wall
RM      = rm -f
BINNAME = a.out
FILES   = mountroot.c filesystem.c util.c

default: all
all: build

build: $(FILES)
	$(GPP) $(CFLAGS) -o $(BINNAME) $(FILES)

run: build
	./$(BINNAME) mydisk

clean:
	$(RM) $(BINNAME)
