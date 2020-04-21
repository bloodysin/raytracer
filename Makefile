.SUFFIXES: .cpp

CXX = g++
CXXFLAGS = -std=c++11 -I./lib -I./include/ -I/usr/X11/include -L./lib/mac/ -Xpreprocessor -fopenmp
LDFLAGS = -lm -lstdc++ -lfreeimage -lomp
ODIR = include
LIBDIR = lib
OBJ = main.o readfile.o Transform.o raytrace.o shader.o

# all: raytrace

# raytrace: main.o readfile.o variables.h
	# $(CXX) $(CXXFLAGS) -o $(OBJ) $(LDFLAGS)

# main.o: main.cpp variables.h
	# $(CXX) $(CXXFLAGS) -c main.cpp

# readfile.o: readfile.cpp readfile.h variables.h
	# $(CXX) $(CXXFLAGS) -c readfile.cpp

%.o: %.cpp %.h
	$(CXX) -c -o $@ $< $(CXXFLAGS)

run: $(OBJ)
	$(CXX) $(CXXFLAGS) $? $(LDFLAGS)
	./a.out ./hw3-submissionscenes/scene7.test
	open test.png

clean:
	rm *.o a.out *.png
