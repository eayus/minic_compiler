all:
	clang++ -g src/*.cpp src/ast/*.cpp src/parser/*.cpp -o bin/main `llvm-config --ldflags` -lboost_iostreams -std=c++17 -Wall -Wextra -Wno-unused-parameter

run: all
	bin/main

