#include "parse_exception.hpp"

ParseException::ParseException(std::string&& error) noexcept :
	error(error) { }

const char* ParseException::what() const noexcept {
	return this->error.c_str();
}
