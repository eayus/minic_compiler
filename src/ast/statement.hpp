#pragma once

#include <memory>
#include <forward_list>
#include "expr.hpp"

namespace ast::declaration {
	struct VarDecl;
}

namespace ast::statement {
	using namespace ast::expr;
	using namespace ast::declaration;

	class Statement {
	};

	struct Block : public Statement {
		std::forward_list<std::unique_ptr<VarDecl>> var_decls;
		std::forward_list<std::unique_ptr<Statement>> statements;
	};

	struct IfElse : public Statement {
		std::unique_ptr<Expr> cond;
		std::unique_ptr<Block> if_true;
		std::unique_ptr<Block> if_false;
	};

	struct While : public Statement {
		std::unique_ptr<Expr> cond;
		std::unique_ptr<Statement> body;
	};

	struct Return : public Statement {
		std::unique_ptr<Expr> return_val; // potentially nullptr
	};

}
