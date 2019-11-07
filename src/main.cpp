#include <iostream>
#include <vector>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/utility/string_ref.hpp>
#include "lexer.hpp"

#include "ast/expr.hpp"
#include "ast/statement.hpp"
#include "ast/declaration.hpp"

#include "parser/parse.hpp"
#include "parser/token_stream.hpp"
#include <memory>

int main(int argc, char** argv) {
	char* filepath = argv[1];
	std::cout << "filepath: " << filepath << std::endl;
	boost::iostreams::mapped_file_source file(filepath);


	lexer::Lexer l(boost::string_ref(file.data(), file.size()));

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
