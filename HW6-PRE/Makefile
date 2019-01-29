GPP     = gcc
CFLAGS  = -m32 -g -Wall
RM      = rm -f
BINNAME = a.out
FILES   = myrcp.c

default: all
all: build

build: $(FILES)
	$(GPP) $(CFLAGS) -o $(BINNAME) $(FILES)

run: build
	./$(BINNAME)

clean:
	$(RM) $(BINNAME)
