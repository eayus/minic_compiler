#pragma once

#include "scope.hpp"
#include "../ast/visitor.hpp"
#include "../ast/declaration.hpp"
#include "../ast/statement.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>

using namespace ast::declaration;
using namespace ast::statement;

class CodeGenerator : public ASTVisitor {
public:
	CodeGenerator();

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

	void cg_block(const Block& block);

	llvm::Type* convert_return_type(ReturnType rt);
	llvm::Type* convert_var_type(VarType vt);
	void print();
	void write_to_file(const char* filepath);

private:
	VarType get_current_expr_type(unsigned int line_num, unsigned int column_num, const char* context);
	void set_expr_type(ReturnType ret_type);


	llvm::LLVMContext context;
	llvm::Module module;
	llvm::IRBuilder<> builder;

	Scope scope;

	llvm::Function* current_function;
	llvm::Value* current_expr;
	boost::optional<VarType> current_expr_type;
	// Variables for managing returns
	llvm::BasicBlock* return_block;
	llvm::Value* return_alloca;
	ReturnType current_return_type;
	bool return_called = false;
};
