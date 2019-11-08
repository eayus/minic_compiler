#include "codegen.hpp"
#include <iostream>
#include <vector>

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Constant.h>

using namespace ast::declaration;

CodeGenerator::CodeGenerator() :
	module("main_module", this->context),
	builder(this->context) { }

llvm::Type* CodeGenerator::convert_return_type(ReturnType rt) {
	switch (rt) {
		case ReturnType::Int: return llvm::Type::getInt32Ty(this->context);
		case ReturnType::Float: return llvm::Type::getFloatTy(this->context);
		case ReturnType::Bool: return llvm::Type::getInt1Ty(this->context);
		case ReturnType::Void: return llvm::Type::getVoidTy(this->context);
	}
}

llvm::Type* CodeGenerator::convert_var_type(VarType vt) {
	switch (vt) {
		case VarType::Int: return llvm::Type::getInt32Ty(this->context);
		case VarType::Float: return llvm::Type::getFloatTy(this->context);
		case VarType::Bool: return llvm::Type::getInt1Ty(this->context);
	}
}

void CodeGenerator::visit_program(const Program& program) {
	for (auto& ext : program.externs) {
		ext->accept_visitor(*this);
	}

	for (auto& decl : program.decls) {
		decl->accept_visitor(*this);
	}
}

void CodeGenerator::visit_extern_decl(const ExternDecl& extern_decl) {
	auto return_type = this->convert_return_type(extern_decl.return_type);

	std::vector<llvm::Type*> param_types;
	for (auto& param : extern_decl.params) {
		auto param_type = convert_var_type(param->type);
		param_types.push_back(param_type);
	}

	auto func_type = llvm::FunctionType::get(return_type, param_types, false);

	llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, extern_decl.name, this->module);
}

void CodeGenerator::visit_var_decl(const VarDecl& var_decl) {
	auto var_type = this->convert_var_type(var_decl.type);

	llvm::Value* gv = new llvm::GlobalVariable(
		this->module,
		var_type,
		false,
		llvm::GlobalVariable::InternalLinkage,
		llvm::Constant::getNullValue(var_type),
		var_decl.name
	);

	this->scope.register_var(var_decl.name, gv);
}

void CodeGenerator::visit_func_decl(const FuncDecl& func_decl) {
	auto return_type = this->convert_return_type(func_decl.return_type);

	std::vector<llvm::Type*> param_types;
	for (auto& param : func_decl.params) {
		auto param_type = convert_var_type(param->type);
		param_types.push_back(param_type);
	}

	auto func_type = llvm::FunctionType::get(return_type, param_types, false);

	this->current_function = llvm::Function::Create(func_type, llvm::Function::InternalLinkage, func_decl.name, this->module);

	func_decl.body->accept_visitor(*this);
}

void CodeGenerator::visit_block(const Block& block) {
	this->current_basic_block = llvm::BasicBlock::Create(this->context, "", this->current_function);
	this->builder.SetInsertPoint(this->current_basic_block);

	this->scope.push_scope();

	for (auto& local_decl : block.var_decls) {
		this->visit_local_decl(*local_decl);
	}

	for (auto& stmt : block.statements) {
		stmt->accept_visitor(*this);
	}

	this->scope.pop_scope();
}

void CodeGenerator::visit_local_decl(const VarDecl& local_decl) {
	auto type = this->convert_var_type(local_decl.type);

	llvm::Value* var = this->builder.CreateAlloca(type);
	this->scope.register_var(local_decl.name, var);
}

void CodeGenerator::visit_expr_stmt(const ExprStmt& expr_stmt) {
	if (expr_stmt.expr == nullptr) return;

	expr_stmt.expr->accept_visitor(*this);
}

void CodeGenerator::print() {
	this->module.print(llvm::outs(), nullptr);
}

void CodeGenerator::visit_unary_expr(const UnaryExpr& unary_expr) {
}

void CodeGenerator::visit_binary_expr(const BinaryExpr& binary_expr) {
}

void CodeGenerator::visit_assign_expr(const AssignExpr& assign_expr) {
}

void CodeGenerator::visit_identifier_expr(const IdentifierExpr& identifier_expr) {
}

void CodeGenerator::visit_func_call_expr(const FuncCallExpr& func_call_expr) {
}

void CodeGenerator::visit_int_expr(const IntExpr& int_expr) {
}

void CodeGenerator::visit_float_expr(const FloatExpr& float_expr) {
}

void CodeGenerator::visit_bool_expr(const BoolExpr& bool_expr) {
}
