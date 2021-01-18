#include Make_linux.inc
#include Make_msys2.inc
include Make_osx.inc

CXXFLAGS = -std=c++14 -fPIC
ifdef DEBUG
CXXFLAGS += -g -O0 -Wall -fbounds-check -pedantic -D_GLIBCXX_DEBUG
else
CXXFLAGS += -O3 -march=native -Wall
endif

ALL=	colonisation_MPI.exe

default:	help

all: $(ALL)

clean:
	@rm -fr src/*.o *.exe src/*~ 

colonisation_MPI.exe:
	$(CXX) $(CXXFLAGS) -o colonisation_MPI.exe src/colonisation_MPI.cpp src/galaxie.cpp src/parametres.cpp $(LIB)