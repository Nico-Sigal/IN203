#include Make.inc
#include Make_msys2.inc
include Make_osx.inc

CXXFLAGS = -std=c++14
ifdef DEBUG
CXXFLAGS += -g -O0 -Wall -fbounds-check -pedantic -D_GLIBCXX_DEBUG
CXXFLAGS2 = CXXFLAGS
else
CXXFLAGS2 = ${CXXFLAGS} -O2 -march=native -Wall
CXXFLAGS += -O3 -march=native -Wall
endif

ALL= syracuse_simple.exe syracuse_orbite.exe simple_automate_cellulaire.exe syracuse_omp.exe

default:	help

all: $(ALL)

clean:
	@rm -fr *.o *.exe *~ *.png

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB)

simple_automate_cellulaire.exe : lodepng_old/lodepng.cpp simple_automate_cellulaire.cpp
	$(CXX) $(CXXFLAGS2) $^ -o $@ $(LIB)	

simple_automate_cellulaire_OMP.exe : lodepng_old/lodepng.cpp simple_automate_cellulaire_OMP.cpp
	$(CXX) $(CXXFLAGS2) $^ -o $@ $(LIB)
help:
	@echo "Available targets : "
	@echo "    all                           : compile all executables"
	@echo "    syracuse_simple.exe           : compile simple syracuse sequence executable"
	@echo "    syracuse_orbite.exe           : compile syracuse with orbit executable"
	@echo "    simple_automate_cellulaire.exe: compile simple cellular automata executable"
	@echo "Add DEBUG=yes to compile in debug"
	@echo "Configuration :"
	@echo "    CXX      :    $(CXX)"
	@echo "    CXXFLAGS :    $(CXXFLAGS)"
