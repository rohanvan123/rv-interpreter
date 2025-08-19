# CXX = g++
# CXXFLAGS = -std=c++20 -Iincludes -Wall -Werror -Wpedantic -Wunused
# TARGET = bin/test
# SRCS = src/main.cpp src/lexer.cpp src/parser.cpp src/arithmetic_parser.cpp src/evaluator.cpp src/utils.cpp src/expression.cpp src/ir_generator.cpp

# test:
# 	mkdir -p bin
# 	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# main:
# 	mkdir -p bin
# 	$(CXX) $(CXXFLAGS) $(SRCS) -o bin/main
# clean:
# 	rm -f $(TARGET)
# CXX = g++
# CXXFLAGS = -std=c++20 -Iincludes -Wall -Werror -Wpedantic -Wunused -MMD -MP

# TARGET = bin/test
# SRC_DIR = src
# OBJ_DIR = bin/obj

# SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
# DEPS = $(OBJS:.o=.d)

# # Default target
# all: $(TARGET)

# # Link the final binary
# $(TARGET): $(OBJS)
# 	@mkdir -p $(dir $@)
# 	$(CXX) $(CXXFLAGS) $^ -o $@

# # Compile each .cpp into .o
# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
# 	@mkdir -p $(dir $@)
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# # Include dependency files
# -include $(DEPS)

# clean:
# 	rm -rf bin

CXX = g++
CXXFLAGS = -std=c++20 -Iincludes -Wall -Werror -Wpedantic -Wunused -MMD -MP

SRC_DIR = src
OBJ_DIR = bin/obj

# All sources
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Targets
MAIN_TARGET = bin/main
TEST_TARGET = bin/test

# Default target builds both
all: $(MAIN_TARGET) $(TEST_TARGET)

# Main executable
$(MAIN_TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Test executable (can reuse the same sources or add test-specific .cpp later)
$(TEST_TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile each .cpp into .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Include dependency files
-include $(DEPS)

clean:
	rm -rf bin