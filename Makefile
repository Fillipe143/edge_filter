.SILENT:

CXX = g++
CXXFLAGS = -std=c++20 -Wall -lraylib

SRCS = src/*.cpp
TARGET = bin/edge_filter

build:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

run: build
	./$(TARGET)
