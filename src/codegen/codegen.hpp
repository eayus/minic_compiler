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
	void visit_unary_expr(const UnaryExpr& unary_expr) override;
	void visit_binary_expr(const BinaryExpr& binary_expr) override;
	void visit_assign_expr(const AssignExpr& assign_expr) override;
	void visit_identifier_expr(const IdentifierExpr& identifier_expr) override;
	void visit_func_call_expr(const FuncCallExpr& func_call_expr) override;
	void visit_int_expr(const IntExpr& int_expr) override;
	void visit_float_expr(const FloatExpr& float_expr) override;
	void visit_bool_expr(const BoolExpr& bool_expr) override;

	llvm::Type* convert_return_type(ReturnType rt);
	llvm::Type* convert_var_type(VarType vt);
	void print();

private:
	llvm::LLVMContext context;
	llvm::Module module;
	llvm::IRBuilder<> builder;

	Scope scope;

	llvm::BasicBlock* current_basic_block;
	llvm::Function* current_function;
};
