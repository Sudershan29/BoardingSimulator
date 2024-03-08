CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra
LDFLAGS :=

OBJDIR := obj
BINDIR := bin

EXECUTABLE := $(BINDIR)/run
SIMULATION := $(BINDIR)/simulation

.PHONY: all clean

run: run.cpp
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $(EXECUTABLE)

simulation: simulation.cpp
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $(SIMULATION)

clean:
	rm -rf $(OBJDIR) $(BINDIR)
