# Compiler and compiler flags
CXX = g++
CXXFLAGS = -Wall -Werror -lglew32 -lglfw3 -lkernel32 -lopengl32 -lglu32 


# Directories
MAIN_DIR = .
UTIL_DIR = util

# Source files
SRC_MAIN = $(wildcard $(MAIN_DIR)/*.cpp)
SRC_UTIL = $(wildcard $(UTIL_DIR)/*.cpp)
SRC = $(SRC_MAIN) $(SRC_UTIL)

# Object files
OBJ = $(SRC:.cpp=.o)

# Target executable
TARGET = run.exe

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean object files and target executable
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

