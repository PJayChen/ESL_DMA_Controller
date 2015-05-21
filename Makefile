SYSTEMC_DIR = /home/pjay/systemc-2.3.1
PROJECT     = DMA
BUILDFLAGS  = -g3
CXX         = g++

INCFLAGS    = -I. -I${SYSTEMC_DIR}/include
LDFLAGS     = -L${SYSTEMC_DIR}/lib-linux64 -lsystemc -lm -Wl,-rpath,${SYSTEMC_DIR}/lib-linux64
SRC_CPPHEAD = dma
SRC_CPP     =
HEADERS     =
MAIN        = main.cpp
OBJECTS     = $(SRC_CPPHEAD:=.o) $(SRC_CPP:cpp=o)

EXE = $(PROJECT)

all: $(EXE)

$(EXE): $(MAIN) $(OBJECTS) $(HEADERS)
	@echo "$@ building..."
	$(CXX) $(INCFLAGS) $(MAIN) $(OBJECTS) $(LDFLAGS) -o $@
	@echo ""
	@echo "$@ build done successfully..."
	@echo ""

%.o:%.cpp %.h
	@echo "Compiling $< ..."
	$(CXX) -c $< $(INCFLAGS)

clean:
	rm -f $(EXE)
	rm -f *.o
	rm -f *.vcd
