#include "lexical_error.hpp"

LexicalError::LexicalError(boost::string_ref bad_input, unsigned int line_num, unsigned int column_num) noexcept :
	bad_input(bad_input),
	line_num(line_num),
	column_num(column_num) {
	this->err_string = std::string("lexical error: at line ")
                 + std::to_string(this->line_num)
		 + " column "
		 + std::to_string(this->column_num)
		 + ", the string \""
		 + std::string(this->bad_input)
		 + "\" is not a valid token.";
}

const char* LexicalError::what() const noexcept {
	return this->err_string.c_str();
}

