#pragma once

// TODO: Lexer considers bitwise vs logical AND/OR, grammar only considers logical.

#include <memory>
#include <vector>
#include <forward_list>
#include <string>
#include <boost/optional.hpp>
#include "../lexer.hpp"
#include "ast.hpp"
#include "type.hpp"

using lexer::Token;
using namespace ast::type;

namespace ast {
namespace expr {

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

	std::vector<FuncType> binary_op_valid_types(BinaryOp op);

	unsigned int binary_op_precedence(BinaryOp op);
	boost::optional<BinaryOp> binary_op_from_token_type(Token::Type type);
	const char* binary_op_to_str(BinaryOp op);

	enum class UnaryOp {
		Not,
		Negate
	};

	const char* unary_op_to_str(UnaryOp op);

	struct Expr : public ASTNode {
		virtual void print_tree(std::string indent_str, bool is_last) const override = 0;
	};

	struct UnaryExpr : public Expr {
		UnaryExpr(UnaryOp op, std::unique_ptr<Expr> operand) noexcept;
		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;

		UnaryOp op;
		std::unique_ptr<Expr> operand;
	};

	struct BinaryExpr : public Expr {
		BinaryExpr(BinaryOp op, std::unique_ptr<Expr> first_operand, std::unique_ptr<Expr> second_operand) noexcept;
		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;

		BinaryOp op;
		std::unique_ptr<Expr> first_operand;
		std::unique_ptr<Expr> second_operand;
	};

	struct AssignExpr : public Expr {
		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;

		std::string name;
		std::unique_ptr<Expr> expr;
	};

	struct IdentifierExpr : public Expr {
		IdentifierExpr(std::string&& name) noexcept;
		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;

		std::string name;
	};

	struct FuncCallExpr : public Expr {
		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;

		std::string func_name;
		std::forward_list<std::unique_ptr<Expr>> params;
	};

	struct IntExpr : public Expr {
		IntExpr(int value) noexcept;
		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;

		int value;
	};

	struct FloatExpr : public Expr {
		FloatExpr(float value) noexcept;
		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;

		float value;
	};

	struct BoolExpr : public Expr {
		BoolExpr(bool value) noexcept;
		void print_tree(std::string indent_str, bool is_last) const override;
		void accept_visitor(ASTVisitor& visitor) override;

		bool value;
	};

}
}
