
CXX = g++
CXXFLAGS = -std=c++20

all: run

run: run.cpp
	$(CXX) $(CXXFLAGS) -o run run.cpp flight.cpp

clean:
	rm -f run
