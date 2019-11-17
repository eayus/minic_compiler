#include "type_error.hpp"
#include "../ast/declaration.hpp"

using ast::declaration::var_type_to_str;

std::string type_list_to_str(const std::vector<VarType>& type_list) {
	std::string result = "(";

	for (size_t i = 0; i < type_list.size() - 1; i++) {
		result += var_type_to_str(type_list[i]);
		result += ", ";
	}

	result += var_type_to_str(type_list.back());
	result += ")";

	return result;
}

std::string expected_types_to_str(const std::vector<std::vector<VarType>>& expected_types) {
	std::string result = "";

	if (expected_types.size() == 1) {
		result += type_list_to_str(expected_types[0]);
	} else {
		for (size_t i = 0; i < expected_types.size() - 1; i++) {
			result += type_list_to_str(expected_types[i]);
			result += ", ";
		}

		result += "or ";
		result += type_list_to_str(expected_types.back());
	}

	return result;
}

TypeError::TypeError(
		unsigned int line_num,
		unsigned int column_num,
		const std::string& callee_name,
		const std::vector<std::vector<VarType>>& expected_types,
		const std::vector<VarType>& actual_type
	) {
	this->err_string = "type error:\nline "
		+ std::to_string(line_num)
		+ " column "
		+ std::to_string(column_num)
		+ ":\n"
		+ "invalid types passed to "
		+ callee_name
		+ ", expected "
		+ expected_types_to_str(expected_types)
		+ "\nreceived "
		+ type_list_to_str(actual_type)
		+ ".";
}


TypeError::TypeError(unsigned int line_num, unsigned int column_num, const std::string& msg) {
	this->err_string = "type error:\nline "
		+ std::to_string(line_num)
		+ " column "
		+ std::to_string(column_num)
		+ ":\n"
		+ msg
		+ ".";
}

const char* TypeError::what() const noexcept {
	return this->err_string.c_str();
}
