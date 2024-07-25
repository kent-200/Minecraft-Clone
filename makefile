# Compiler and compiler flags
CXX = g++
CXXFLAGS = -Wall -Werror
LDFLAGS = -lglfw3 -lkernel32 -lopengl32 -lglu32 -lglew32

# Source files directories and wildcard for all .cpp files
SRC_DIRS = . util
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# Object files directory and naming
OBJ_DIR = obj
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(notdir $(SRCS:.cpp=.o)))

# Executable name
EXEC = run.exe

# Default target
all: $(EXEC) run

# Linking object files into the executable
$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Compiling each source file into object files
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir $(OBJ_DIR)

# Clean rule to remove compiled files and executable
clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(EXEC)

# Phony target to run the executable
run: $(EXEC)
	.\$(EXEC)

# Phony targets
.PHONY: clean all run
