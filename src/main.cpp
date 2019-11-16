#include <iostream>
#include <vector>
#include <boost/utility/string_ref.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "lexer/lexer.hpp"
#include <fstream>
#include <sstream>

#include "ast/expr.hpp"
#include "ast/statement.hpp"
#include "ast/declaration.hpp"

#include "parser/parse.hpp"
#include "parser/token_stream.hpp"
#include <memory>

#include "codegen/codegen.hpp"

int main(int argc, char** argv) {
	char* filepath = argv[1];
	std::cout << "filepath: " << filepath << std::endl;
	boost::iostreams::mapped_file_source file(filepath);


	lexer::Lexer l(boost::string_ref(file.data(), file.size()));

	/*std::ifstream file(filepath);
	std::stringstream file_buf;
	file_buf << file.rdbuf();
	boost::string_ref file_contents = file_buf.str();

	lexer::Lexer l(file_contents);*/

	//std::vector<lexer::Token> tokens;
	TokenStream ts;

	l.lex(ts.tokens);

	for (auto t : ts.tokens) {
		t.print();
	}

	Parser p(ts);
	auto prog = p.parse_program();

	prog->print_tree("", true);

	CodeGenerator cg;
	prog->accept_visitor(cg);
	cg.print();
	cg.write_to_file("output.ll");
	
	file.close();
}
