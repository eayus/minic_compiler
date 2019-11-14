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
			return map_iter->second.val;
		}
	}

	return nullptr;
}

boost::optional<VarType> Scope::lookup_variable_type(const std::string& s) {
	for (auto it = this->frames.rbegin();
	          it != this->frames.rend();
		  it++) {
		
		auto map_iter = it->find(s);
		if (map_iter != it->end()) {
			return map_iter->second.type;
		}
	}

	return boost::none;
}

boost::optional<std::pair<ReturnType, std::forward_list<VarType>>> Scope::lookup_func_type(const std::string& s) {
	auto map_iter = this->func_types.find(s);
	if (map_iter != this->func_types.end()) {
		return map_iter->second;
	}

	return boost::none;
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

void Scope::register_func_type(const std::string& name, ReturnType ret_type, std::forward_list<VarType> param_types) {
	this->func_types.insert({ name, { ret_type, param_types } });
}
