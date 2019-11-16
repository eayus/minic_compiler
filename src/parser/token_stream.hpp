#pragma once

#include "../lexer/lexer.hpp"

using namespace lexer;

class TokenStream {
public:
	TokenStream() noexcept;
	Token::Type peek_type(size_t num_ahead) noexcept;
	const Token& next() noexcept;
	unsigned int current_line() noexcept;
	unsigned int current_column() noexcept;

	std::vector<Token> tokens;
	size_t index;
};
