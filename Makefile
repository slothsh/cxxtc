CXX := c++
INCLUDE := -I./src
FLAGS := -Wall -Wpedantic -Wextra -std=c++23
BUILD_DIR = ./build

SRC_TESTS := $(wildcard ./tests/*.test.cpp)
TEST_EXES = $(wildcard $(BUILD_DIR)/test_*)

configure:
	@mkdir -p $(BUILD_DIR)

tests: $(SRC_TESTS)
	$(CXX) $(FLAGS) $(INCLUDE) -include ./tests/test.hpp -o $(BUILD_DIR)/$(patsubst tests/%.test.cpp,test_%,$<) ./tests/test.cpp $<

.PHONY: tests

run_tests: $(TEST_EXES)
	@./$<

.PHONY: run_tests

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: clean
