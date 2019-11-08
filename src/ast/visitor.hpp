#pragma once

#include "../ast/declaration.hpp"
#include "../ast/statement.hpp"
#include "../ast/expr.hpp"

using namespace ast::declaration;
using namespace ast::statement;
using namespace ast::expr;

class ASTVisitor {
public:
	virtual void visit_program(const Program&) = 0;
	virtual void visit_extern_decl(const ExternDecl&) = 0;
	virtual void visit_var_decl(const VarDecl&) = 0;
	virtual void visit_func_decl(const FuncDecl&) = 0;
	virtual void visit_block(const Block&) = 0;
	virtual void visit_local_decl(const VarDecl&) = 0;
	virtual void visit_expr_stmt(const ExprStmt&) = 0;
	virtual void visit_unary_expr(const UnaryExpr&) = 0;
	virtual void visit_binary_expr(const BinaryExpr&) = 0;
	virtual void visit_assign_expr(const AssignExpr&) = 0;
	virtual void visit_identifier_expr(const IdentifierExpr&) = 0;
	virtual void visit_func_call_expr(const FuncCallExpr&) = 0;
	virtual void visit_int_expr(const IntExpr&) = 0;
	virtual void visit_float_expr(const FloatExpr&) = 0;
	virtual void visit_bool_expr(const BoolExpr&) = 0;
};
