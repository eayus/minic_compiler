all:
	clang++ -g src/*.cpp src/ast/*.cpp src/parser/*.cpp src/codegen/*.cpp -o bin/main `llvm-config --ldflags --system-libs --libs all` -lboost_iostreams -std=c++17 -Wall -Wextra -Wno-unused-parameter

run: all
	bin/main

