run: all
	bin/main

all:
	clang++ -g src/*.cpp src/ast/*.cpp src/parser/*.cpp -o bin/main -lboost_iostreams -std=c++17 -Wall -Wextra
