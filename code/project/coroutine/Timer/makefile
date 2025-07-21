# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

INCLUDE_DIRS := -I.. -I../Fiber -I../Thread -I../Schduler

# Target executable
TARGET := timer_test

# Source files and object files
SRC_DIRS := . ../Fiber ../Thread
SRCS_ALL := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.cpp))
CORE_SRCS := $(filter-out %_test.cpp, $(SRCS_ALL))  # Exclude test files
SRCS = $(CORE_SRCS) $(wildcard *_test.cpp)  # Include test files
OBJS := $(SRCS:.cpp=.o)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -o $@ $^

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

.PHONY: all clean
# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)
