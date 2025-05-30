
# thelet.shevach@gmail.com


CXX       := g++
CXXFLAGS  := -std=c++20 -Wall -Wextra -pedantic -Iinclude
SFML_LIB  := -lsfml-graphics -lsfml-window -lsfml-system

SRC_CORE  := src/core
SRC_GUI   := src/gui
OBJ_DIR   := build

# ─── gather sources ─────────────────────────────────────────────────────────
CORE_SRCS := $(wildcard $(SRC_CORE)/*.cpp)
GUI_SRCS  := $(wildcard $(SRC_GUI)/*.cpp)
DEMO_SRC  := demo/Demo.cpp
TEST_SRCS := $(wildcard tests/*.cpp)

# ─── map .cpp → build/.../.o ────────────────────────────────────────────────
CORE_OBJS := $(CORE_SRCS:%.cpp=$(OBJ_DIR)/%.o)
GUI_OBJS  := $(GUI_SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEMO_OBJ  := $(DEMO_SRC:%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJS := $(TEST_SRCS:%.cpp=$(OBJ_DIR)/%.o)

.PHONY: all Main Gui Tests valgrind clean

all: Main

# tell make where to look for source files
vpath %.cpp src/core src/gui demo tests

# ─── compile any build/.../*.o from its corresponding %.cpp ────────────────
$(OBJ_DIR)/%.o : %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ─── link the console demo ─────────────────────────────────────────────────
Main: $(CORE_OBJS) $(DEMO_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ─── link the SFML GUI ──────────────────────────────────────────────────────
Gui: $(CORE_OBJS) $(GUI_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIB)

# ─── build & run unit tests ─────────────────────────────────────────────────
Tests: $(CORE_OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	./Tests

# ─── convenience targets ────────────────────────────────────────────────────
valgrind: Main
	valgrind --leak-check=full ./Main

clean:
	@echo "Cleaning build artifacts"
	@rm -rf $(OBJ_DIR) Main Gui Tests
