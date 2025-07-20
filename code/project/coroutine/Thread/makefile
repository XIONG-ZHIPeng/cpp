# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -O2

# Target executable
TARGET := thread_test

# Source files and object files
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all clean
# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)
