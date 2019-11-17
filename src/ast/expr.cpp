#include "expr.hpp"
#include "visitor.hpp"
#include "../lexer/lexer.hpp"
#include <iostream>
#include <iomanip>

namespace ast {
namespace expr {

	UnaryExpr::UnaryExpr(UnaryOp op, std::unique_ptr<Expr> operand, unsigned int line_num, unsigned int column_num) noexcept :
		op(op),
		operand(std::move(operand)),
		line_num(line_num),
		column_num(column_num) { }

	BinaryExpr::BinaryExpr(BinaryOp op, std::unique_ptr<Expr> first_operand, std::unique_ptr<Expr> second_operand, unsigned int line_num, unsigned int column_num) noexcept :
		op(op),
		first_operand(std::move(first_operand)),
		second_operand(std::move(second_operand)),
		line_num(line_num),
		column_num(column_num) { }

	IdentifierExpr::IdentifierExpr(std::string&& name) noexcept :
		name(std::move(name)) { }

	IntExpr::IntExpr(int value) noexcept :
		value(value) { }

	FloatExpr::FloatExpr(float value) noexcept :
		value(value) { }

	BoolExpr::BoolExpr(bool value) noexcept :
		value(value) { }

	std::vector<FuncType> binary_op_func_type(BinaryOp op) {
		switch (op) {
			case BinaryOp::Multiply:
			case BinaryOp::Divide:
			case BinaryOp::Plus:
			case BinaryOp::Minus:
				return {
					FuncType::binary(VarType::Int, VarType::Int, ReturnType::Int),
					FuncType::binary(VarType::Float, VarType::Float, ReturnType::Float)
				};

			case BinaryOp::Modulo:
				return {
					FuncType::binary(VarType::Int, VarType::Int, ReturnType::Int)
				};

			case BinaryOp::Less:
			case BinaryOp::LessEqual:
			case BinaryOp::Greater:
			case BinaryOp::GreaterEqual:
				return {
					FuncType::binary(VarType::Int, VarType::Int, ReturnType::Bool),
					FuncType::binary(VarType::Float, VarType::Float, ReturnType::Bool)
				};

			case BinaryOp::Equals:
			case BinaryOp::NotEquals:
				return {
					FuncType::binary(VarType::Int, VarType::Int, ReturnType::Bool),
					FuncType::binary(VarType::Float, VarType::Float, ReturnType::Bool),
					FuncType::binary(VarType::Bool, VarType::Bool, ReturnType::Bool),
				};

			case BinaryOp::And:
			case BinaryOp::Or:
				return {
					FuncType::binary(VarType::Bool, VarType::Bool, ReturnType::Bool)
				};
		}
	}

	unsigned int binary_op_precedence(BinaryOp op) {
		switch (op) {
			case BinaryOp::Multiply: return 40;
			case BinaryOp::Divide: return 40;
			case BinaryOp::Modulo: return 40;
			case BinaryOp::Plus: return 30;
			case BinaryOp::Minus: return 30;
			case BinaryOp::Less: return 20;
			case BinaryOp::LessEqual: return 20;
			case BinaryOp::Greater: return 20;
			case BinaryOp::GreaterEqual: return 20;
			case BinaryOp::Equals: return 10;
			case BinaryOp::NotEquals: return 10;
			case BinaryOp::And: return 5;
			case BinaryOp::Or: return 0;
		}
	}

	boost::optional<BinaryOp> binary_op_from_token_type(Token::Type type) {
		switch (type) {
			case Token::Type::Asterisk: return BinaryOp::Multiply;
			case Token::Type::Divide: return BinaryOp::Divide;
			case Token::Type::Percent: return BinaryOp::Modulo;
			case Token::Type::Plus: return BinaryOp::Plus;
			case Token::Type::Minus: return BinaryOp::Minus;
			case Token::Type::Less: return BinaryOp::Less;
			case Token::Type::LessEqual: return BinaryOp::LessEqual;
			case Token::Type::Greater: return BinaryOp::Greater;
			case Token::Type::GreaterEqual: return BinaryOp::GreaterEqual;
			case Token::Type::Equals: return BinaryOp::Equals;
			case Token::Type::NotEqual: return BinaryOp::NotEquals;
			case Token::Type::LogicAnd: return BinaryOp::And;
			case Token::Type::LogicOr: return BinaryOp::Or;
			default: return boost::none;
		}
	}

	const char* binary_op_to_str(BinaryOp op) {
		switch (op) {
			case BinaryOp::Multiply: return "multiply";
			case BinaryOp::Divide: return "divide";
			case BinaryOp::Modulo: return "modulo";
			case BinaryOp::Plus: return "plus";
			case BinaryOp::Minus: return "minus";
			case BinaryOp::Less: return "less";
			case BinaryOp::LessEqual: return "less_or_equal";
			case BinaryOp::Greater: return "greater";
			case BinaryOp::GreaterEqual: return "greater_or_equal";
			case BinaryOp::Equals: return "equal";
			case BinaryOp::NotEquals: return "not_equal";
			case BinaryOp::And: return "and";
			case BinaryOp::Or: return "or";
		}
	}

	const char* binary_op_symbol(BinaryOp op) {
		switch (op) {
			case BinaryOp::Multiply: return "*";
			case BinaryOp::Divide: return "/";
			case BinaryOp::Modulo: return "%";
			case BinaryOp::Plus: return "+";
			case BinaryOp::Minus: return "-";
			case BinaryOp::Less: return "<";
			case BinaryOp::LessEqual: return "<=";
			case BinaryOp::Greater: return ">";
			case BinaryOp::GreaterEqual: return ">=";
			case BinaryOp::Equals: return "==";
			case BinaryOp::NotEquals: return "!=";
			case BinaryOp::And: return "&&";
			case BinaryOp::Or: return "||";

		}
	}

	const char* unary_op_to_str(UnaryOp op) {
		switch (op) {
			case UnaryOp::Not: return "not";
			case UnaryOp::Negate: return "negate";
		}
	}

	void UnaryExpr::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- unary_expr"
			<< " { op: "
			<< unary_op_to_str(this->op)
			<< " }\n";

		indent_str += "   ";
		this->operand->print_tree(indent_str, true);
	}

	void BinaryExpr::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- binary_expr"
			<< " { op: "
			<< binary_op_to_str(this->op)
			<< " }\n";

		indent_str += "   ";
		this->first_operand->print_tree(indent_str, true);
		this->second_operand->print_tree(indent_str, true);
	}
	
	void AssignExpr::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- assign_expr"
			<< " { name: "
			<< this->name
			<< " }\n";

		indent_str += "   ";
		this->expr->print_tree(indent_str, true);
	}

	void IdentifierExpr::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- variable"
			<< " { name: "
			<< this->name
			<< " }\n";
	}

	void FuncCallExpr::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- func_call"
			<< " { func_name: "
			<< this->func_name
			<< " }\n";

		indent_str += "   ";

		for (auto it = this->params.begin(); it != this->params.end(); it++) {
			(*it)->print_tree(indent_str, false);
		}
	}

	void IntExpr::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- int"
			<< " { value: "
			<< this->value
			<< " }\n";
	}
	void FloatExpr::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- float"
			<< " { value: "
			<< this->value
			<< " }\n";
	}
	void BoolExpr::print_tree(std::string indent_str, bool is_last) const {
		std::cout << indent_str
			<< "+- bool"
			<< " { value: "
			<< std::boolalpha
			<< this->value
			<< " }\n";
	}

	void UnaryExpr::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_unary_expr(*this);
	}

	void BinaryExpr::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_binary_expr(*this);
	}

	void AssignExpr::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_assign_expr(*this);
	}

	void IdentifierExpr::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_identifier_expr(*this);
	}

	void FuncCallExpr::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_func_call_expr(*this);
	}

	void IntExpr::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_int_expr(*this);
	}

	void FloatExpr::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_float_expr(*this);
	}

	void BoolExpr::accept_visitor(ASTVisitor& visitor) {
		visitor.visit_bool_expr(*this);
	}

}
}
