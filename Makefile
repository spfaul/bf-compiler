CXX = g++
CXX_FLAGS = -Wall -Wextra -pedantic -std=c++11
EXEC_OUT_PATH = ./bf
SRCS = ./src/main.cpp

.PHONY: test

build:
	$(CXX) $(CXX_FLAGS) $(SRCS) -o $(EXEC_OUT_PATH)

test:
	./bf.out
	fasm hello.asm hello
	./hello