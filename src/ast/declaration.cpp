#include "declaration.hpp"
#include <iostream>
#include "visitor.hpp"

namespace ast {
namespace declaration {

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

	void Program::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_program(*this);
	}

	void FuncDecl::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_func_decl(*this);
	}

	void VarDecl::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_var_decl(*this);
	}

	void ExternDecl::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_extern_decl(*this);
	}

	void Param::accept_visitor(ASTVisitor& visitor) {
		//visitor.visit_program(*this);
	}
	
}
}
