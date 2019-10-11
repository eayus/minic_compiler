#pragma once

#include <vector>
#include <memory>
#include <variant>
#include <utility>
#include <string>


// TODO: Lexer considers bitwise vs logical AND/OR, grammar only considers logical.

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

struct Expr {
	std::variant<float, int, bool, UnaryExpr, BinaryExpr, AssignExpr> self;
};

struct UnaryExpr {
	UnaryOp op;
	std::unique_ptr<Expr> operand;
};

struct BinaryExpr {
	BinaryOp op;
	std::unique_ptr<Expr> first_operand;
	std::unique_ptr<Expr> second_operand;
};

struct AssignExpr {
	std::string name;
	std::unique_ptr<Expr> expr;
};

/*enum Expr {
	Float(f32),
	Int(i32),
	Bool(bool),

	Binary(BinaryOp, Box<Expr>, Box<Expr>),
	Unary(UnaryOp, Box<Expr>),
	
	Assign(String, Box<Expr>),
}*/
