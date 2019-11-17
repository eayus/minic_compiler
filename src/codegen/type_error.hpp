#pragma once

#include <exception>
#include <string>
#include <vector>
#include "../ast/declaration.hpp"

using ast::declaration::VarType;

class TypeError : public std::exception {
public:
	TypeError(
		unsigned int line_num,
		unsigned int column_num,
		const std::string& callee_name,
		const std::vector<std::vector<VarType>>& expected_types,
		const std::vector<VarType>& actual_type
	);
	TypeError(unsigned int line_num, unsigned int column_num, const std::string& msg);
	const char* what() const noexcept override;

private:
	std::string err_string;

};
