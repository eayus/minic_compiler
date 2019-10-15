run: all
	bin/main

all:
	clang++ src/*.cpp src/ast/*.cpp -o bin/main -lboost_iostreams -std=c++17
