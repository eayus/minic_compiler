#pragma once

#include <exception>
#include <string>

class ParseException : public std::exception {
public:
	ParseException(std::string&& error) noexcept;
	const char* what() const noexcept override;

private:
	std::string error;
};
