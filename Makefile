# ============================================================
#  Makefile  –  Build the Roguelike Game
#  Usage:
#    make        → compile everything
#    make run    → compile and run
#    make clean  → delete compiled files
# ============================================================

CXX     = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# All source files
SRCS = main.cpp Item.cpp

# Output executable name
TARGET = roguelike

# Default target
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)
	@echo "Build successful! Run with: ./roguelike"

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) savegame.txt
