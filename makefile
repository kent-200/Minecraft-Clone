#-----------------------------------------
# Unified Makefile (no duplicate run, no circular deps)
#-----------------------------------------

# 1) Default goal
.DEFAULT_GOAL := all
.PHONY: all clean start

# 2) Platform detection
ifeq ($(OS),Windows_NT)
  PLATFORM := WINDOWS
else ifeq ($(shell uname -s),Darwin)
  PLATFORM := MACOS
else
  PLATFORM := LINUX
endif

# 3) Directories & sources
SRC      := src
IMGUI    := lib/imgui
BACKENDS := $(IMGUI)/backends
SRCS     := $(wildcard $(SRC)/*.cpp) \
            $(IMGUI)/imgui.cpp $(IMGUI)/imgui_draw.cpp \
            $(IMGUI)/imgui_tables.cpp $(IMGUI)/imgui_widgets.cpp \
            $(BACKENDS)/imgui_impl_glfw.cpp \
            $(BACKENDS)/imgui_impl_opengl3.cpp

OBJDIR   := obj
OBJS     := $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(SRCS)))

# 4) Compiler & flags per platform
ifeq ($(PLATFORM),WINDOWS)
  CXX      := g++
  CXXFLAGS := -Wall -I$(IMGUI) -Ilib \
              -lglfw3 -lkernel32 -lopengl32 -lglu32 -lglew32 -lwinmm
  LDLIBS   :=
else ifeq ($(PLATFORM),MACOS)
  CXX        := g++
  PKG_CONFIG := pkg-config
  BREW_PFX   := $(shell brew --prefix)
  CXXFLAGS   := -Wall -std=c++11 -I$(IMGUI) \
                $(shell $(PKG_CONFIG) --cflags glew glfw3) \
                -I$(BREW_PFX)/include
  LDLIBS     := $(shell $(PKG_CONFIG) --libs glew glfw3) -framework OpenGL
else
  CXX        := g++
  PKG_CONFIG := pkg-config
  CXXFLAGS   := -Wall -std=c++11 -I$(IMGUI) \
                $(shell $(PKG_CONFIG) --cflags glew glfw3)
  LDLIBS     := $(shell $(PKG_CONFIG) --libs glew glfw3) -lGL
endif

# 5) Executable name (must differ from 'start')
EXEC := run

# 6) Build all
all: $(EXEC)

$(EXEC): $(OBJS) | $(OBJDIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

# 7) Compilation rules
$(OBJDIR)/%.o: $(SRC)/%.cpp | $(OBJDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
$(OBJDIR)/%.o: $(IMGUI)/%.cpp | $(OBJDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
$(OBJDIR)/%.o: $(BACKENDS)/%.cpp | $(OBJDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# 8) Ensure obj dir
$(OBJDIR):
	mkdir -p $(OBJDIR)

# 9) Single start target (no name collision!)
start: $(EXEC)
	@echo "Launching $(EXEC)…"
ifeq ($(PLATFORM),WINDOWS)
	.\$(EXEC)
else
	./$(EXEC)
endif

# 10) Cleanup
clean:
ifeq ($(PLATFORM),WINDOWS)
	powershell -Command "Remove-Item -Recurse -Force $(OBJDIR)\*; Remove-Item -Force $(EXEC).exe"
else
	rm -rf $(OBJDIR) $(EXEC)
endif
