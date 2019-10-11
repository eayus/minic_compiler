#include <iostream>
#include <vector>
#include <boost/iostreams/device/mapped_file.hpp>
#include "lexer.hpp"

int main() {
	boost::iostreams::mapped_file_source file("test.txt");


	lexer::Lexer l(std::string_view(file.data(), file.size()));

	std::vector<lexer::Token> tokens;

	l.lex(tokens);

	for (auto t : tokens) {
		t.print();
	}
	
	file.close();
}
