#pragma once

#include "ast.hpp"
#include "statement.hpp"
#include <forward_list>
#include <string>

namespace ast::declaration {
	
	using namespace ast::statement;

	enum class VarType {
		Int = 0,
		Float = 1,
		Bool = 2
	};

	enum class ReturnType {
		Int = 0,
		Float = 1,
		Bool = 2,
		Void = 3
	};

	struct Param : public ASTNode {
		VarType type;
		std::string name;

		void print_tree(std::string indent_str, bool is_last) const override;
	};

	class Declaration : public ASTNode {
	};

	struct ExternDecl : public Declaration {
		ReturnType return_type;
		std::string name;
		std::forward_list<std::unique_ptr<Param>> params;

		void print_tree(std::string indent_str, bool is_last) const override;
	};

	struct VarDecl : public Declaration {
		VarType type;
		std::string name;
		
		void print_tree(std::string indent_str, bool is_last) const override;
	};

	struct FuncDecl : public Declaration {
		ReturnType return_type;
		std::string name;
		std::forward_list<std::unique_ptr<Param>> params;
		std::unique_ptr<Block> body;

		void print_tree(std::string indent_str, bool is_last) const override;
	};

	struct Program : public ASTNode {
		std::forward_list<std::unique_ptr<ExternDecl>> externs;
		std::forward_list<std::unique_ptr<Declaration>> decls;

		void print_tree(std::string indent_str, bool is_last) const override;
	};

	const char* return_type_to_str(ReturnType rt);
	const char* var_type_to_str(VarType vt);

}
