# Construire manuală a proiectului Pacman (Linux sau terminal MSYS2).
#   make          - compilează jocul în build/pacman
#   make run      - compilează și pornește jocul
#   make test     - compilează și rulează testele
#   make clean    - șterge folderul build

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS := -Iinclude -MMD -MP

ifeq ($(OS),Windows_NT)
EXE := .exe
endif

BUILD  := build
TARGET := $(BUILD)/pacman$(EXE)
TEST   := $(BUILD)/smoke_test$(EXE)

LIB_SRC := src/Types.cpp src/GameEngine.cpp src/Renderer.cpp src/Listener.cpp
LIB_OBJ := $(LIB_SRC:src/%.cpp=$(BUILD)/%.o)
MAIN_OBJ := $(BUILD)/main.o
TEST_OBJ := $(BUILD)/smoke_test.o
DEPS := $(LIB_OBJ:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(LIB_OBJ) $(MAIN_OBJ)
	$(CXX) $^ -o $@

$(TEST): $(LIB_OBJ) $(TEST_OBJ)
	$(CXX) $^ -o $@

$(BUILD)/%.o: src/%.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD)/smoke_test.o: tests/smoke_test.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

run: $(TARGET)
	./$(TARGET)

test: $(TEST)
	./$(TEST)

clean:
	rm -rf $(BUILD)

-include $(DEPS)
