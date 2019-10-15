#pragma once

// TODO: Lexer considers bitwise vs logical AND/OR, grammar only considers logical.

#include <memory>
#include <string>

namespace ast::expr {

	enum class BinaryOp {
		Multiply,
		Divide,
		Modulo,
		Plus,
		Minus,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equals,
		NotEquals,
		And,
		Or
	};

	enum class UnaryOp {
		Not,
		Negate
	};

	class Expr {
	};

	struct UnaryExpr : public Expr {
		UnaryExpr(UnaryOp op, std::unique_ptr<Expr> operand) noexcept;

		UnaryOp op;
		std::unique_ptr<Expr> operand;
	};

	struct BinaryExpr : public Expr {
		BinaryExpr(BinaryOp op, std::unique_ptr<Expr> first_operand, std::unique_ptr<Expr> second_operand) noexcept;

		BinaryOp op;
		std::unique_ptr<Expr> first_operand;
		std::unique_ptr<Expr> second_operand;
	};

	struct AssignExpr : public Expr {
		AssignExpr(std::string&& name, std::unique_ptr<Expr> expr) noexcept;

		std::string name;
		std::unique_ptr<Expr> expr;
	};

	struct IntExpr : public Expr {
		IntExpr(int value) noexcept;

		int value;
	};

	struct FloatExpr : public Expr {
		FloatExpr(float value) noexcept;

		float value;
	};

	struct BoolExpr : public Expr {
		BoolExpr(bool value) noexcept;

		bool value;
	};

}
