#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include <forward_list>
#include <llvm/IR/Value.h>
#include <boost/optional.hpp>
#include "../ast/declaration.hpp"

using namespace ast::declaration;

struct VariableEntry {
	llvm::Value* val;
	VarType type;
};

class Scope {
public:
	Scope();
	llvm::Value* lookup_variable_val(const std::string& s);
	boost::optional<VarType> lookup_variable_type(const std::string& s);
	boost::optional<std::pair<ReturnType, std::forward_list<VarType>>> lookup_func_type(const std::string& s);
	void push_scope();
	void pop_scope();
	void register_var(const std::string& name, llvm::Value* value, VarType);
	void register_func_type(const std::string& name, ReturnType ret_type, std::forward_list<VarType> param_types);
	bool function_exists(const std::string& name);

private:
	std::vector<std::unordered_map<std::string, VariableEntry>> frames;
	std::unordered_map<std::string, std::pair<ReturnType, std::forward_list<VarType>>> func_types;
};
