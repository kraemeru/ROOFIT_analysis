# Variables
CC=g++ -std=c++11
INC=$(PWD)/include

#LDIR=$(PWD)/lib
#KP_CFLAGS=-I/home/mengqing/kpix/kpix -I/home/mengqing/kpix/generic
#KP_LFLAGS=-L$(LDIR) -lkpix 
#KP_RLIB=-Wl,-rpath=$(LDIR)
binaries=$(patsubst $(SRC)/%.cxx,$(BIN)/%,$(wildcard $(SRC)/*.cxx))

CFLAGS=-Wall $(shell root-config --cflags)
LFLAGS=$(shell root-config --glibs) -I$(ROOTSYS)/include -L$(ROOTSYS)/lib -lRooFitCore -lRooFit

# dir:
SRC=$(PWD)/src
BIN=$(PWD)/bin
OBJ=$(PWD)/.obj


#default:
all: $(binaries)

$(BIN)/%: $(SRC)/%.cxx
	$(CC) -o $@ $< $(CFLAGS) $(LFLAGS)


clean:
	rm $(BIN)/*
