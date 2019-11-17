#pragma once

#include <string>
#include "visitor.hpp"

using namespace ast::declaration;
using namespace ast::statement;
using namespace ast::expr;

class TreePrinter : public ASTVisitor {
public:
	TreePrinter() = default;

	std::string indent_str() const;

	void visit_program(const Program& program) override;
	void visit_extern_decl(const ExternDecl& extern_decl) override;
	void visit_var_decl(const VarDecl& decl) override;
	void visit_func_decl(const FuncDecl& decl) override;
	void visit_block(const Block& block) override;
	void visit_local_decl(const VarDecl& local_decl) override;
	void visit_expr_stmt(const ExprStmt& expr_stmt) override;
	void visit_return_stmt(const Return& ret_stmt) override;
	void visit_if_else_stmt(const IfElse& if_else_stmt) override;
	void visit_while_stmt(const While& while_stmt) override;
	void visit_unary_expr(const UnaryExpr& unary_expr) override;
	void visit_binary_expr(const BinaryExpr& binary_expr) override;
	void visit_assign_expr(const AssignExpr& assign_expr) override;
	void visit_identifier_expr(const IdentifierExpr& identifier_expr) override;
	void visit_func_call_expr(const FuncCallExpr& func_call_expr) override;
	void visit_int_expr(const IntExpr& int_expr) override;
	void visit_float_expr(const FloatExpr& float_expr) override;
	void visit_bool_expr(const BoolExpr& bool_expr) override;

private:
	void print_param(const Param& param);
	unsigned int indent_level;
};
