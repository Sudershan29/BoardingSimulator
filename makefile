CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra
LDFLAGS :=

OBJDIR := obj
BINDIR := bin

EXECUTABLE := $(BINDIR)/run

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): run.cpp
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)
