#include "tree_printer.hpp"
#include "declaration.hpp"

using ast::declaration::var_type_to_str;
using ast::declaration::return_type_to_str;
using ast::expr::unary_op_to_str;
using ast::expr::binary_op_to_str;

std::string TreePrinter::indent_str() const {
	std::string result = "";

	for (int i = 0; i < this->indent_level; i++) {
		result += "   ";
	}

	return result;
}

void TreePrinter::print_param(const Param& param) {
	std::cout
		<< this->indent_str()
		<< "+- param"
		<< " { "
		<< "type: " << var_type_to_str(param.type) << ", "
		<< "name: " << param.name
		<< " }"
		<< std::endl;
}

void TreePrinter::visit_program(const Program& program) {
	this->indent_level = 0;

	std::cout
		<< this->indent_str()
		<< "+- program"
		<< std::endl;

	this->indent_level++;

		for (auto& ext : program.externs) {
			ext->accept_visitor(*this);
		}

		for (auto& decl : program.decls) {
			decl->accept_visitor(*this);
		}

	this->indent_level--;
}


void TreePrinter::visit_extern_decl(const ExternDecl& extern_decl) {
	std::cout
		<< this->indent_str()
		<< "+- extern { "
		<< "name: " << extern_decl.name << ", "
		<< "return_type: " << return_type_to_str(extern_decl.return_type)
		<< " }"
		<< std::endl;

	this->indent_level++;

	for (auto& param : extern_decl.params) {
		this->print_param(*param);
	}

	this->indent_level--;
}

void TreePrinter::visit_var_decl(const VarDecl& decl) {
	std::string s = indent_str();

	std::cout
		<< s
		<< "+- var_decl { "
		<< "type: " << var_type_to_str(decl.type) << ", "
		<< "name: " << decl.name
		<< " }"
		<< std::endl;
}

void TreePrinter::visit_func_decl(const FuncDecl& decl) {
	std::string s = indent_str();

	std::cout
		<< s
		<< "+- function { "
		<< "name: " << decl.name << ", "
		<< "return_type: " << return_type_to_str(decl.return_type)
		<< " }"
		<< std::endl;

	this->indent_level++;

		for (auto& param : decl.params) {
			this->print_param(*param);
		}

		decl.body->accept_visitor(*this);

	this->indent_level--;
}

void TreePrinter::visit_block(const Block& block) {
	std::string s = this->indent_str();

	std::cout
		<< s
		<< "+- block"
		<< std::endl;

	this->indent_level++;
	
	for (auto& var_decl : block.var_decls) {
		var_decl->accept_visitor(*this);
	}

	for (auto& stmt : block.statements) {
		stmt->accept_visitor(*this);
	}

	this->indent_level--;
}

void TreePrinter::visit_local_decl(const VarDecl& decl) {
	std::string s = indent_str();

	std::cout
		<< s
		<< "+- var_decl { "
		<< "type: " << var_type_to_str(decl.type) << ", "
		<< "name: " << decl.name
		<< " }"
		<< std::endl;
}

void TreePrinter::visit_expr_stmt(const ExprStmt& expr_stmt) {
	std::string s = indent_str();

	std::cout
		<< s
		<< "+- expression statement"
		<< std::endl;

	this->indent_level++;

	if (expr_stmt.expr != nullptr) {
		expr_stmt.expr->accept_visitor(*this);
	}

	this->indent_level--;
}

void TreePrinter::visit_return_stmt(const Return& ret_stmt) {
	std::cout
		<< this->indent_str()
		<< "+- return"
		<< std::endl;

	this->indent_level++;

	if (ret_stmt.return_val != nullptr) {
		ret_stmt.return_val->accept_visitor(*this);
	}

	this->indent_level--;
}

void TreePrinter::visit_if_else_stmt(const IfElse& if_else_stmt) {
	std::cout
		<< this->indent_str()
		<< "+- if_stmt"
		<< std::endl;

	this->indent_level++;
	
		// Condition
		std::cout
			<< this->indent_str()
			<< "+- condition"
			<< std::endl;
		
		this->indent_level++;
		if_else_stmt.cond->accept_visitor(*this);
		this->indent_level--;

		// If true
		std::cout
			<< this->indent_str()
			<< "+- if_true"
			<< std::endl;

		this->indent_level++;
		if_else_stmt.if_true->accept_visitor(*this);
		this->indent_level--;

		// If false
		if (if_else_stmt.if_false != nullptr) {
			std::cout
				<< this->indent_str()
				<< "+- if_false"
				<< std::endl;

			this->indent_level++;
			if_else_stmt.if_false->accept_visitor(*this);
			this->indent_level--;
		}

	this->indent_level--;
}

void TreePrinter::visit_while_stmt(const While& while_stmt) {
	std::cout
		<< this->indent_str()
		<< "+- while_loop"
		<< std::endl;

	this->indent_level++;
	
		// Condition
		std::cout
			<< this->indent_str()
			<< "+- condition"
			<< std::endl;

		this->indent_level++;
			while_stmt.cond->accept_visitor(*this);
		this->indent_level--;

		// Body
		std::cout
			<< this->indent_str()
			<< "+- body"
			<< std::endl;

		this->indent_level++;
			while_stmt.body->accept_visitor(*this);
		this->indent_level--;

	this->indent_level--;
}

void TreePrinter::visit_unary_expr(const UnaryExpr& unary_expr) {
	std::cout
		<< this->indent_str()
		<< "+- unary_expr"
		<< " { "
		<< "op: " << unary_op_to_str(unary_expr.op)
		<< " }"
		<< std::endl;
	
	this->indent_level++;
		unary_expr.operand->accept_visitor(*this);
	this->indent_level--;
}

void TreePrinter::visit_binary_expr(const BinaryExpr& binary_expr) {
	std::cout
		<< this->indent_str()
		<< "+- binary_expr"
		<< " { "
		<< "op: " << binary_op_to_str(binary_expr.op)
		<< " }"
		<< std::endl;

	this->indent_level++;
		binary_expr.first_operand->accept_visitor(*this);
		binary_expr.second_operand->accept_visitor(*this);
	this->indent_level--;
}

void TreePrinter::visit_assign_expr(const AssignExpr& assign_expr) {
	std::cout
		<< this->indent_str()
		<< "+- assignment"
		<< " { "
		<< "name: " << assign_expr.name
		<< " }"
		<< std::endl;

	this->indent_level++;
		assign_expr.expr->accept_visitor(*this);
	this->indent_level--;
}

void TreePrinter::visit_identifier_expr(const IdentifierExpr& identifier_expr) {
	std::cout
		<< this->indent_str()
		<< "+- identifier"
		<< " { "
		<< "name: " << identifier_expr.name
		<< " }"
		<< std::endl;
}

void TreePrinter::visit_func_call_expr(const FuncCallExpr& func_call_expr) {
	std::cout
		<< this->indent_str()
		<< "+- func_call"
		<< " { "
		<< "func_name: " << func_call_expr.func_name
		<< " }"
		<< std::endl;

	this->indent_level++;
		for (auto& param : func_call_expr.params) {
			param->accept_visitor(*this);
		}
	this->indent_level--;
}

void TreePrinter::visit_int_expr(const IntExpr& int_expr) {
	std::cout
		<< this->indent_str()
		<< "+- int"
		<< " { "
		<< "value: " << int_expr.value
		<< " }"
		<< std::endl;
}

void TreePrinter::visit_float_expr(const FloatExpr& float_expr) {
	std::cout
		<< this->indent_str()
		<< "+- float"
		<< " { "
		<< "value: " << float_expr.value
		<< " }"
		<< std::endl;
}

void TreePrinter::visit_bool_expr(const BoolExpr& bool_expr) {
	std::cout
		<< this->indent_str()
		<< "+- bool"
		<< " { "
		<< "value: " << bool_expr.value
		<< " }"
		<< std::endl;
}
