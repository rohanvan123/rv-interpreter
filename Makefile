CXX = g++
CXXFLAGS = -std=c++20 -w
TARGET = bin/test
SRCS = src/main.cpp

test:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)