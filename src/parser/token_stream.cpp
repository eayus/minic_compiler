#include "token_stream.hpp"

TokenStream::TokenStream() noexcept :
	index(0) { }

Token::Type TokenStream::peek_type(size_t num_ahead) noexcept {
	size_t pos = this->index + num_ahead - 1;

	if (pos >= this->tokens.size()) {
		return Token::Type::EndOfInput;
	}

	return this->tokens[pos].type;
}

const Token& TokenStream::next() noexcept {
	static Token END_OF_INPUT_TOKEN(Token::Type::EndOfInput, "");

	if (this->index >= this->tokens.size()) {
		return END_OF_INPUT_TOKEN;
	}

	return this->tokens[this->index++];
}
