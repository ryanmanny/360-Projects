COMP    = cc
CFLAGS  = -Wall
RM      = rm -f
BINNAME = mycgi.bin
FILES   = mycgi.c util.o

default: build

build: $(FILES)
	$(COMP) $(CFLAGS) -o $(BINNAME) $(FILES)
	chmod u+s mycgi.bin

clean:
	$(RM) $(BINNAME)
