#include "expr.hpp"

namespace ast::expr {

	UnaryExpr::UnaryExpr(UnaryOp op, std::unique_ptr<Expr> operand) noexcept :
		op(op),
		operand(std::move(operand)) { }

	BinaryExpr::BinaryExpr(BinaryOp op, std::unique_ptr<Expr> first_operand, std::unique_ptr<Expr> second_operand) noexcept :
		op(op),
		first_operand(std::move(first_operand)),
		second_operand(std::move(second_operand)) { }

	AssignExpr::AssignExpr(std::string&& name, std::unique_ptr<Expr> expr) noexcept :
		name(std::move(name)),
		expr(std::move(expr)) { }

	IntExpr::IntExpr(int value) noexcept :
		value(value) { }

	FloatExpr::FloatExpr(float value) noexcept :
		value(value) { }

	BoolExpr::BoolExpr(bool value) noexcept :
		value(value) { }

}

