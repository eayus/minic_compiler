#include "declaration.hpp"
#include <iostream>

namespace ast::declaration {

	const char* return_type_to_str(ReturnType rt) {
		switch (rt) {
			case ReturnType::Int: return "int";
			case ReturnType::Float: return "float";
			case ReturnType::Bool: return "bool";
			case ReturnType::Void: return "void";
		}
	}

	const char* var_type_to_str(VarType vt) {
		switch (vt) {
			case VarType::Int: return "int";
			case VarType::Float: return "float";
			case VarType::Bool: return "bool";
		}
	}

	void Program::print_tree(std::string indent_str, bool is_last) const {
		std::cout << "+- program\n";

		indent_str = "   ";

		auto it = this->externs.begin();
		while (true) {
			auto& ext = *it;

			it++;
			
			if (it == this->externs.end()) {
				ext->print_tree(indent_str, true);
				break;
			} else {
				ext->print_tree(indent_str, false);
			}
		}

		auto it2 = this->decls.begin();
		while (true) {
			auto& decl = *it2;

			it2++;

			if (it2 == this->decls.end()) {
				decl->print_tree(indent_str, true);
				break;
			} else {
				decl->print_tree(indent_str, false);
			}
		}

		std::cout << std::flush;
	}

	void ExternDecl::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- extern { "
			<< "name: " << this->name << ", "
			<< "return_type: " << return_type_to_str(this->return_type)
			<< " }\n";

		indent_str += is_last ? "   " : "|  ";


		auto it = this->params.begin();
		while (true) {
			auto& param = *it;

			it++;

			if (it == this->params.end()) {
				param->print_tree(indent_str, true);
				break;
			} else {
				param->print_tree(indent_str, false);
			}
		}
	}

	void Param::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- param { "
			<< "type: " << var_type_to_str(this->type) << ", "
			<< "name: " << this->name
			<< " }\n";
	}


	void VarDecl::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- variable { "
			<< "type: " << var_type_to_str(this->type) << ", "
			<< "name: " << this->name
			<< " }\n";
	}


	void FuncDecl::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- function { "
			<< "name: " << this->name << ", "
			<< "return_type: " << return_type_to_str(this->return_type)
			<< "}\n";

		indent_str += is_last ? "   " : "|  ";

		auto it = this->params.begin();
		while (true) {
			auto& param = *it;

			it++;

			if (it == this->params.end()) {
				param->print_tree(indent_str, false);
				break;
			} else {
				param->print_tree(indent_str, false);
			}
		}

		this->body->print_tree(indent_str, true);
	}
	
}
