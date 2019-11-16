#include "parse_error.hpp"
#include "../lexer/token.hpp"

ParseError::ParseError(const std::string& err_string) : err_string(err_string) { }

ParseError::ParseError(unsigned int line_num, unsigned int column_num, const std::string& context, const std::string& expected_string, const std::vector<Token::Type> expected_types, const Token& unexpected_token) noexcept :
	line_num(line_num),
	column_num(column_num),
	context(context),
	expected_string(expected_string),
	expected_types(expected_types),
	unexpected_token(unexpected_token) {
	this->err_string = std::string("parse error:\n\tline ")
		+ std::to_string(this->line_num)
		+ " column "
		+ std::to_string(this->column_num)
		+ ":\n\tin "
		+ this->context
		+ ":\n\texpected "
		+ this->expected_string
		+ "\n\tvalid tokens: ";


	if (this->expected_types.size() == 1) {
		this->err_string += Token::type_to_str(this->expected_types[0]);
	} else {
		for (size_t i = 0; i < this->expected_types.size() - 1; i++) {
			this->err_string += std::string(Token::type_to_str(this->expected_types[i])) + ", ";
		}

		this->err_string += std::string("or ") + Token::type_to_str(this->expected_types.back());
	}


	this->err_string += std::string("\n\tinstead found the ")
		+ unexpected_token.to_string();
}

const char* ParseError::what() const noexcept {
	return this->err_string.c_str();
}
