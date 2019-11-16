#pragma once

#include <vector>
#include <boost/utility/string_ref.hpp>
#include "token.hpp"

namespace lexer {
	class Lexer {
	public:
		Lexer(boost::string_ref str);
		void lex(std::vector<Token>& tokens);

	private:
		Token next_token();
	
	private:
		size_t i;
		int last_newline_index;
		unsigned int current_line;
		boost::string_ref str;
		std::vector<Token> tokens;
	};
}
