CXX := c++
INCLUDE := -I./src
FLAGS := -Wall -Wpedantic -Wextra -std=c++23

all:
	$(CXX) $(FLAGS) $(INCLUDE) -o timecode ./examples/timecode.example.cpp
