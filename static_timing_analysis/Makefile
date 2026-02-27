.PHONY: all run clean

CXX = g++
CXXFLAGS = -Isrc -std=c++11 -DPRINTTIME=true -DNDEBUG 
OPTFLAGS = -flto -funroll-loops -finline-functions -O3 -ffast-math
WARNINGS = -g -Wall

SRCDIR = src
OBJDIR = obj
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.d,$(SRCS))

# Testcase file
CASENAME = c17# change here
CASEDIR = testcase/$(CASENAME)

NETLIST = $(CASENAME).v
LIBRARY = testcase/test_lib.lib
PATTERN = $(CASENAME).pat

TARGET = sta

all: $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJS)
	$(CXX) $(WARNINGS) $(CXXFLAGS) $(OPTFLAGS) $^ -o $@

-include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(WARNINGS) $(CXXFLAGS) $(OPTFLAGS) -MMD -c $< -o $@

run: $(TARGET)
	time ./$(TARGET) $(CASEDIR)/$(NETLIST) -l $(LIBRARY) -i $(CASEDIR)/$(PATTERN)

clean:
	rm -rf $(OBJDIR) $(TARGET) *.txt