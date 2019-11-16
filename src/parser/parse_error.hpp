#pragma once

#include <exception>
#include <string>
#include <vector>
#include "../lexer/token.hpp"

using namespace lexer;

class ParseError : public std::exception {
public:
	ParseError(const std::string& err_string);
	ParseError(unsigned int line_num, unsigned int column_num, const std::string& context, const std::string& expected_string, const std::vector<Token::Type> expected_types, const Token& unexpected_token) noexcept;
	const char* what() const noexcept override;

private:
	std::string expected_string;
	std::vector<Token::Type> expected_types;
	Token unexpected_token;
	unsigned int line_num;
	unsigned int column_num;
	std::string context; // What logical place the error is taking place, e.g. "param list"
	std::string err_string;
};
