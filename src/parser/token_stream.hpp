#pragma once

#include "../lexer.hpp"
#include <optional>

using namespace lexer;

class TokenStream {
public:
	TokenStream() noexcept;
	Token::Type peek_type(size_t num_ahead) noexcept;
	const Token& next() noexcept;

	std::vector<Token> tokens;
	size_t index;
};
