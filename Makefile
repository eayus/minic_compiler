run: all
	bin/main

all:
	clang++ src/*.cpp -o bin/main -lboost_iostreams -std=c++17
