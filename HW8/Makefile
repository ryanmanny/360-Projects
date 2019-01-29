COMP    = cc
CFLAGS  = -g -Wall
RM      = rm -f
CLIENTBINNAME = client.bin
SERVERBINNAME = server.bin
CLIENTFILES = client.c
SERVERFILES = server.c

default: build

build: server client

server: $(SERVERFILES)
	$(COMP) $(CFLAGS) -o $(SERVERBINNAME) $(SERVERFILES)

client: $(CLIENTFILES)
	$(COMP) $(CFLAGS) -o $(CLIENTBINNAME) $(CLIENTFILES)

clean:
	$(RM) $(CLIENTBINNAME) $(SERVERBINNAME)
