#pragma once

#include "ast.hpp"
#include "statement.hpp"
#include <forward_list>
#include <string>
#include <vector>
#include "type.hpp"

namespace ast {
namespace declaration {
	
	using namespace ast::statement;
	using namespace ast::type;

	
	struct Param : public ASTNode {
		VarType type;
		std::string name;

		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;
	};

	class Declaration : public ASTNode {
	};

	struct ExternDecl : public Declaration {
		ReturnType return_type;
		std::string name;
		std::forward_list<std::unique_ptr<Param>> params;
		unsigned int line_num;
		unsigned int column_num;

		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;
	};

	struct VarDecl : public Declaration {
		VarType type;
		std::string name;
		
		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;
	};

	struct FuncDecl : public Declaration {
		ReturnType return_type;
		std::string name;
		std::forward_list<std::unique_ptr<Param>> params;
		std::unique_ptr<Block> body;
		unsigned int line_num;
		unsigned int column_num;

		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;
	};

	struct Program : public ASTNode {
		std::forward_list<std::unique_ptr<ExternDecl>> externs;
		std::forward_list<std::unique_ptr<Declaration>> decls;

		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;
	};

	const char* return_type_to_str(ReturnType rt);
	const char* var_type_to_str(VarType vt);

}
}
