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
	if (this->index >= this->tokens.size()) {
		return this->tokens.back();
	}

	return this->tokens[this->index++];
}

unsigned int TokenStream::current_line() noexcept {
	if (this->index >= this->tokens.size()) {
		return this->tokens.back().line_num;
	} else {
		return this->tokens[this->index].line_num;
	}
}

unsigned int TokenStream::current_column() noexcept {
	if (this->index >= this->tokens.size()) {
		return this->tokens.back().column_num;
	} else {
		return this->tokens[this->index].column_num;
	}
}
