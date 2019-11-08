#include "scope.hpp"

Scope::Scope() {
	this->push_scope(); // Add the global scope
}

llvm::Value* Scope::lookup_variable(const std::string& s) {
	for (auto it = this->frames.rbegin();
	          it != this->frames.rend();
		  it++) {
		
		auto map_iter = it->find(s);
		if (map_iter != it->end()) {
			return map_iter->second;
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

void Scope::register_var(const std::string& name, llvm::Value* value) {
	this->frames.back().insert({ name, value });
}
