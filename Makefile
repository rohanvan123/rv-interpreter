CXX = g++
CXXFLAGS = -std=c++20
TARGET = bin/test
SRCS = src/main.cpp

test:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

main:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRCS) -o bin/main
clean:
	rm -f $(TARGET)