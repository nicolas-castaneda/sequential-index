# Determine the operating system
ifeq ($(OS),Windows_NT)
    RM = del /Q
    EXECUTABLE_EXTENSION = .exe
else
    RM = rm -f
    EXECUTABLE_EXTENSION =
endif

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17
INCLUDE_DIRS = -I.

# Source files and output executable
SRCS = main.cpp
HEADERS = sequential_index.hpp data.hpp index.hpp libraries.hpp record.hpp response.hpp sequential_index_header.hpp sequential_index_utils.hpp sequential_index_record.hpp binary_search_response.hpp utils.hpp
OUTPUT = program$(EXECUTABLE_EXTENSION)

# Object files (automatically generated)
OBJS = $(SRCS:.cpp=.o)

# Targets
all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -o $@ $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(OUTPUT)

.PHONY: all clean
