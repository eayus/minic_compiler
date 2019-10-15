#pragma once

#include "../token.hpp"
#include <optional>

class TokenStream {
public:
	TokenStream() noexcept;
	Token::Type peek_type(size_t num_ahead) noexcept;
	Token& next_token() noexcept;

private:
	const std::vector<Token> tokens;
	size_t index;
};
