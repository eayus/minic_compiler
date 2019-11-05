#pragma once

#include <memory>
#include <forward_list>
#include "expr.hpp"
#include "ast.hpp"

namespace ast::declaration {
	struct VarDecl;
}

namespace ast::statement {
	using namespace ast::expr;
	using namespace ast::declaration;

	struct Statement : public ASTNode {
		virtual void print_tree(std::string indent_str, bool is_last) const override = 0;
	};

	struct Block : public Statement {
		std::forward_list<std::unique_ptr<VarDecl>> var_decls;
		std::forward_list<std::unique_ptr<Statement>> statements;

		void print_tree(std::string indent_str, bool is_last) const override;
	};

	struct IfElse : public Statement {
		std::unique_ptr<Expr> cond;
		std::unique_ptr<Block> if_true;
		std::unique_ptr<Block> if_false; // potentially nullptr

		void print_tree(std::string indent_str, bool is_last) const override;
	};

	struct While : public Statement {
		std::unique_ptr<Expr> cond;
		std::unique_ptr<Statement> body;

		void print_tree(std::string indent_str, bool is_last) const override;
	};

	struct Return : public Statement {
		std::unique_ptr<Expr> return_val; // potentially nullptr

		void print_tree(std::string indent_str, bool is_last) const override;
	};

	struct ExprStmt : public Statement {
		std::unique_ptr<Expr> expr; //potentially nullptr

		void print_tree(std::string indent_str, bool is_last) const override;
	};

}
