#include <iostream>
#include <vector>
#include <boost/iostreams/device/mapped_file.hpp>
#include "lexer.hpp"

#include "ast/expr.hpp"
#include "ast/statement.hpp"
#include "ast/declaration.hpp"

#include "parser/parse.hpp"
#include "parser/token_stream.hpp"
#include <memory>

int main() {
	boost::iostreams::mapped_file_source file("test.txt");


	lexer::Lexer l(std::string_view(file.data(), file.size()));

	//std::vector<lexer::Token> tokens;
	TokenStream ts;

	l.lex(ts.tokens);

	for (auto t : ts.tokens) {
		t.print();
	}

	auto prog = parse_program(ts);

	prog->print_tree("", true);
	
	file.close();
}
