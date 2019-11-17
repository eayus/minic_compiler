#include "statement.hpp"
#include "declaration.hpp"
#include "visitor.hpp"

#include <iostream>

// TODO: with this iterating method there is a bug which means it segfaults on empty lists, it is only solvied in the "Block" print_tree method.

namespace ast {
namespace statement {

	void Block::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_block(*this);
	}

	void IfElse::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_if_else_stmt(*this);
	}

	void While::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_while_stmt(*this);
	}

	void Return::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_return_stmt(*this);
	}

	void ExprStmt::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_expr_stmt(*this);
	}

}
}
