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

	void Block::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- block\n";

		indent_str += is_last ? "   " : "|  ";

		auto it = this->var_decls.begin();
		while (true) {
			if (it == this->var_decls.end()) break;
			auto& decl = *it;

			it++;

			if (it == this->var_decls.end()) {
				decl->print_tree(indent_str, true);
				break;
			} else {
				decl->print_tree(indent_str, false);
			}
		}

		auto it2 = this->statements.begin();
		while (true) {
			if (it2 == this->statements.end()) break;
			auto& stmt = *it2;

			it2++;

			if (it2 == this->statements.end()) {
				stmt->print_tree(indent_str, true);
				break;
			} else {
				stmt->print_tree(indent_str, false);
			}
		}
	}


	void IfElse::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- if_statement\n";

		indent_str += is_last ? "   " : "|  ";

		std::cout << indent_str
			<< "+- condition\n";
		this->cond->print_tree(indent_str + "|  ", false);
		
		std::cout << indent_str
			<< "+- if_true\n";
		this->if_true->print_tree(indent_str + "|  ", false);

		if (this->if_false != nullptr) {
			std::cout << indent_str
				<< "+- if_false\n";
			this->if_false->print_tree(indent_str + "   ", true);
		}
	}

	void While::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- while_loop\n";

		indent_str += is_last ? "   " : "|  ";

		std::cout << indent_str
			<< "+- condition\n";
		this->cond->print_tree(indent_str + "|   ", false);

		std::cout << indent_str
			<< "+ body\n";
		this->body->print_tree(indent_str + "    ", true);
	}

	void Return::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- return\n";

		indent_str += is_last ? "   " : "|  ";

		if (this->return_val != nullptr) {
			this->return_val->print_tree(indent_str, true);
		}
	}
		
	void ExprStmt::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- expression statement\n";

		indent_str += is_last ? "   " : "|   ";

		if (this->expr != nullptr) {
			this->expr->print_tree(indent_str, true);
		}
	}

}
}
