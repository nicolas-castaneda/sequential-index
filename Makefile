# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17
INCLUDE_DIRS = -I.

# Source files and output executable
SRCS = main.cpp SequentialIndex.cpp
HEADERS = SequentialIndex.h SequentialIndexHeader.h SequentialIndexRecord.h utils.h libraries.h
OUTPUT = my_program

# Object files (automatically generated)
OBJS = $(SRCS:.cpp=.o)

# Targets
all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -o $@ $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUTPUT)

.PHONY: all clean
