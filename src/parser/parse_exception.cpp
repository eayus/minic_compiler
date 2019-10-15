#include "parse_exception.cpp"

ParseException::ParseException(std::string&& error) noexcept :
	error(error) { }

const char* ParseException::what() const noexcept override {
	return this->error.c_str();
}
