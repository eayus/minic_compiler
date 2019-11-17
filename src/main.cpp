#include <iostream>
#include <vector>
#include <boost/utility/string_ref.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "lexer/lexer.hpp"
#include <fstream>
#include <sstream>
#include <exception>

#include "ast/expr.hpp"
#include "ast/statement.hpp"
#include "ast/declaration.hpp"
#include "ast/tree_printer.hpp"

#include "parser/parse.hpp"
#include "parser/token_stream.hpp"
#include <memory>

#include "codegen/codegen.hpp"

int main(int argc, char** argv) {
	// Parse command line arguments

	// If no parameters are supplied
	if (argc == 1) {
		std::cerr << "usage error: supply a minic file to compile as a command line argument!" << std::endl;
		return 1;
	}

	// If more than 1 parameter is supplied
	if (argc > 2) {
		std::cerr << "usage error: only supply a single minic file as command line argument!" << std::endl;
		return -1;
	}

	char* filepath = argv[1];


	try {
		// Memory map the file we want to compile. This allows for fast iteration during lexing.
		boost::iostreams::mapped_file_source file(filepath);
	
		// Lex the file into our token stream.
		lexer::Lexer l(boost::string_ref(file.data(), file.size()));
		TokenStream ts;
		l.lex(ts.tokens);

		// Parse the program into AST
		Parser p(ts);
		auto prog = p.parse_program();

		// Print AST
		TreePrinter tp;
		prog->accept_visitor(tp);

		// Generate code into "output.ll"
		CodeGenerator cg;
		prog->accept_visitor(cg);
		cg.write_to_file("output.ll");
		
		file.close();
	} catch (const std::exception& e) {
		// Catch any exceptions we may have thrown during lexing, parsing or code generation, and print it out
		std::cout << e.what() << std::endl;
	}
}
