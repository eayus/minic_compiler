#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include <forward_list>

#include <llvm/IR/Value.h>

class Scope {
public:
	Scope();
	llvm::Value* lookup_variable_val(const std::string& s);
	VarType lookup_variable_type(const std::string& s);
	void push_scope();
	void pop_scope();
	void register_var(const std::string& name, llvm::Value* value, VarType);

private:
	std::vector<std::unordered_map<std::string, std::pair<llvm::Value*, VarType>>> frames;
	std::unordered_map<std::string, std::pair<ReturnType, std::forward_list<VarType>>> func_types;
};
