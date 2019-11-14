all:
	clang++ -g src/*.cpp src/ast/*.cpp src/parser/*.cpp src/codegen/*.cpp -o mccomp `llvm-config --cxxflags --ldflags --system-libs --libs all` -fexceptions -lboost_iostreams

mccomp: all

clean:
	rm -f mccomp

