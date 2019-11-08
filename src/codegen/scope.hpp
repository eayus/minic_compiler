#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include <llvm/IR/Value.h>

class Scope {
public:
	Scope();
	llvm::Value* lookup_variable(const std::string& s);
	void push_scope();
	void pop_scope();
	void register_var(const std::string& name, llvm::Value* value);

private:
	std::vector<std::unordered_map<std::string, llvm::Value*>> frames;
};
