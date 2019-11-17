#pragma once

#include <memory>
#include <forward_list>
#include "expr.hpp"
#include "ast.hpp"

namespace ast {
namespace declaration {
	struct VarDecl;
}
}

namespace ast {
namespace statement {
	using namespace ast::expr;
	using namespace ast::declaration;

	struct Statement : public ASTNode {
	};

	struct Block : public Statement {
		std::forward_list<std::unique_ptr<VarDecl>> var_decls;
		std::forward_list<std::unique_ptr<Statement>> statements;

		void accept_visitor(ASTVisitor& visitor) override;
	};

	struct IfElse : public Statement {
		std::unique_ptr<Expr> cond;
		std::unique_ptr<Block> if_true;
		std::unique_ptr<Block> if_false; // potentially nullptr
		unsigned int line_num;
		unsigned int column_num;

		void accept_visitor(ASTVisitor& visitor) override;
	};

	struct While : public Statement {
		std::unique_ptr<Expr> cond;
		std::unique_ptr<Statement> body;
		unsigned int line_num;
		unsigned int column_num;

		void accept_visitor(ASTVisitor& visitor) override;
	};

	struct Return : public Statement {
		std::unique_ptr<Expr> return_val; // potentially nullptr
		unsigned int line_num;
		unsigned int column_num;

		void accept_visitor(ASTVisitor& visitor) override;
	};

	struct ExprStmt : public Statement {
		std::unique_ptr<Expr> expr; //potentially nullptr

		void accept_visitor(ASTVisitor& visitor) override;
	};

}
}
