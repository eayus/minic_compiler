#include "scope.hpp"

Scope::Scope() {
	this->push_scope(); // Add the global scope
}

llvm::Value* Scope::lookup_variable_val(const std::string& s) {
	for (auto it = this->frames.rbegin();
	          it != this->frames.rend();
		  it++) {
		
		auto map_iter = it->find(s);
		if (map_iter != it->end()) {
			return map_iter->second.second;
		}
	}

	return nullptr;
}

VarType Scope::lookup_variable_type(const std::string& s) {
	for (auto it = this->frames.rbegin();
	          it != this->frames.rend();
		  it++) {
		
		auto map_iter = it->find(s);
		if (map_iter != it->end()) {
			return map_iter->second.first;
		}
	}

	return nullptr;
}

void Scope::push_scope() {
	this->frames.emplace_back();
}

void Scope::pop_scope() {
	this->frames.pop_back();
}

void Scope::register_var(const std::string& name, llvm::Value* value, VarType type) {
	this->frames.back().insert({ name, { value, type } });
}
