###################################################################
# This Makefile was created using the bat-project script
# for project yap_test
# bat-project is part of Bayesian Analysis Toolkit (BAT).
# BAT can be downloaded from http://mpp.mpg.de/bat
###################################################################
#
# Run 'make' to compile the program and 'make clean' to remove
# all compiled parts and 'clean' the directory.
#
# You might need to adjust the CXXFLAGS and LIBS based on
# the BAT installation on your system. Consult the gmake manual
# for details.
#
###################################################################

# List of all class (models) sources used in the program
# Add classes to the end. Backslash indicates continuation
# on the next line
CXXSRCS = bat_gen.cxx d3pi.cxx dkkpi.cxx D_K0pi0pi0.cxx

# List of all program sources used in the program
# Add classes to the end. Backslash indicates continuation
# on the next line
PRGSRCS = runBatGen.cxx

# compiler and flags
CXX       = g++
CXXFLAGS  = -std=c++11 -g -O2 -Wall -fPIC -Wno-deprecated  -Wno-missing-braces #-fopenmp
LD        = /usr/bin/ld -m elf_x86_64
LDFLAGS   = -g -O2  #-fopenmp

# ----------------------------------------------------------------------
# The following definitions rely on the script bat-config being
# available in $PATH. If BAT is not installed in the standard system
# directories, update $PATH accordingly.

CXXFLAGS += `bat-config --cflags` -I$(YAPDIR)/include -I$(ROOTSYS)/include
LIBS := `bat-config --libs` $(YAPDIR)/build/src/libYAP.a $(ROOTSYS)/lib/libPhysics.so

# ----------------------------------------------------------------------
# don't change lines below unless you know what you're doing
#

CXXOBJS = $(addsuffix .o,$(basename $(CXXSRCS)))
MYPROGS = $(basename $(PRGSRCS))
PRGOBJS = $(addsuffix .o,$(basename $(PRGSRCS)))

GARBAGE = $(CXXOBJS) $(PRGOBJS) link.d $(MYPROGS)

# targets
all : $(MYPROGS)

.PHONY : all clean print

link.d : $(addsuffix .h,$(basename $(CXXSRCS))) $(CXXSRCS) $(PRGSRCS)
	$(CXX) -MM $(CXXFLAGS) $(filter-out %.h,$^) > link.d;
	@$(foreach prog,$(MYPROGS), echo $(prog) : $(prog).o >> link.d;)

-include link.d

$(CXXOBJS) $(PRGOBJS) :
	$(CXX) $(CXXFLAGS) -c $(filter $(basename $@).%,$(filter-out %.h,$^)) -o $@

$(MYPROGS) : $(CXXOBJS)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

clean :
	rm -f $(GARBAGE)

print :
	@echo compiler  : $(CXX)
	@echo c++ srcs  : $(CXXSRCS) $(PRGSRCS)
	@echo c++ objs  : $(CXXOBJS) $(PRGOBJS)
	@echo c++ flags : $(CXXFLAGS)
	@echo ld flags  : $(LDFLAGS)
	@echo libs      : $(LIBS)
