CXX = g++
CXXFLAGS = -std=c++20 -Iincludes -Wall -Werror -Wpedantic -Wunused
TARGET = bin/test
SRCS = src/main.cpp src/lexer.cpp src/parser.cpp src/arithmetic_parser.cpp src/evaluator.cpp src/utils.cpp src/expression.cpp

test:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

main:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRCS) -o bin/main
clean:
	rm -f $(TARGET)