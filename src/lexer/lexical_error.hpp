#pragma once

#include <exception>
#include <boost/utility/string_ref.hpp>

class LexicalError : public std::exception {
public:
	LexicalError(boost::string_ref bad_input, unsigned int line_num, unsigned int column_num) noexcept;
	const char* what() const noexcept override;

private:
	boost::string_ref bad_input;
	unsigned int line_num;
	unsigned int column_num;
	std::string err_string;
};
