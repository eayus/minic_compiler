#include "codegen.hpp"
#include <iostream>
#include <vector>

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Verifier.h>

//TODO: when typechecking, enforce that void functions have a "RetVoid" at the end.
//TODO: if statements without an else
//TODO: if assign statements work in 'if' and 'while' conditions

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

	llvm::verifyModule(this->module, &llvm::errs());
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

	auto body = llvm::BasicBlock::Create(this->context, func_decl.name + ":entry_point", this->current_function);
	this->builder.SetInsertPoint(body);

	this->scope.push_scope();
	auto llvm_arg = this->current_function->arg_begin();
	for (auto& param : func_decl.params) {
		llvm_arg->setName(param->name);
		this->scope.register_var(param->name, llvm_arg);
		llvm_arg++;
	}

	this->cg_block(*func_decl.body);

	this->scope.pop_scope();

	llvm::verifyFunction(*this->current_function, &llvm::errs());
}

void CodeGenerator::cg_block(const Block& block) {
	for (auto& local_decl : block.var_decls) {
		this->visit_local_decl(*local_decl);
	}

	for (auto& stmt : block.statements) {
		stmt->accept_visitor(*this);
	}
}

void CodeGenerator::visit_block(const Block& block) {
	this->scope.push_scope();

	cg_block(block);

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

void CodeGenerator::visit_return_stmt(const Return& ret_stmt) {
	if (ret_stmt.return_val == nullptr) {
		this->builder.CreateRetVoid();
	} else {
		ret_stmt.return_val->accept_visitor(*this);
		this->builder.CreateRet(this->current_expr);
	}
}

void CodeGenerator::visit_if_else_stmt(const IfElse& if_else_stmt) {
	auto if_true_block = llvm::BasicBlock::Create(this->context, "if_true");
	auto if_false_block = llvm::BasicBlock::Create(this->context, "if_false");
	auto if_cont_block = llvm::BasicBlock::Create(this->context, "if_cont");


	// Gen condition
	this->scope.push_scope();
	if_else_stmt.cond->accept_visitor(*this);
	this->builder.CreateCondBr(this->current_expr, if_true_block, if_false_block);

	// Gen 'if_true'
	this->builder.SetInsertPoint(if_true_block);
	if_else_stmt.if_true->accept_visitor(*this);
	this->builder.CreateBr(if_cont_block);

	this->scope.pop_scope();

	// Gen 'if_false'
	this->scope.push_scope();

	this->builder.SetInsertPoint(if_false_block);
	if_else_stmt.if_false->accept_visitor(*this);
	this->builder.CreateBr(if_cont_block);

	this->scope.pop_scope();

	
	// Link together
	auto& block_list = this->current_function->getBasicBlockList();
	block_list.push_back(if_true_block);
	block_list.push_back(if_false_block);
	block_list.push_back(if_cont_block);

	this->builder.SetInsertPoint(if_cont_block);
}

void CodeGenerator::visit_while_stmt(const While& while_stmt) {
	auto cond_check_block = llvm::BasicBlock::Create(this->context, "while_cond_check");
	auto body_block = llvm::BasicBlock::Create(this->context, "while_body");
	auto cont_block = llvm::BasicBlock::Create(this->context, "while_cont");

	this->scope.push_scope();

	// Gen condition
	this->builder.SetInsertPoint(cond_check_block);
	while_stmt.cond->accept_visitor(*this);
	this->builder.CreateCondBr(this->current_expr, body_block, cont_block);

	// Gen body
	this->builder.SetInsertPoint(body_block);
	while_stmt.body->accept_visitor(*this);
	this->builder.CreateBr(cond_check_block);

	this->scope.pop_scope();

	// Link together
	auto& block_list = this->current_function->getBasicBlockList();
	block_list.push_back(cond_check_block);
	block_list.push_back(body_block);
	block_list.push_back(cont_block);

	this->builder.SetInsertPoint(cont_block);
}

void CodeGenerator::print() {
	this->module.print(llvm::outs(), nullptr);
}

void CodeGenerator::visit_unary_expr(const UnaryExpr& unary_expr) {
	unary_expr.operand->accept_visitor(*this);

	switch (unary_expr.op) {
		case UnaryOp::Not: this->current_expr = this->builder.CreateNot(this->current_expr);
		case UnaryOp::Negate: this->current_expr = this->builder.CreateNeg(this->current_expr);
	}
}

void CodeGenerator::visit_binary_expr(const BinaryExpr& binary_expr) {
	binary_expr.first_operand->accept_visitor(*this);
	llvm::Value* lhs = this->current_expr;

	binary_expr.second_operand->accept_visitor(*this);
	llvm::Value* rhs = this->current_expr;

	std::cerr << "lhs type: ";
	lhs->getType()->print(llvm::errs());
	std::cerr << std::endl;

	std::cerr << "rhs type: ";
	rhs->getType()->print(llvm::errs());
	std::cerr << std::endl;

	switch (binary_expr.op) {
		case BinaryOp::Multiply: this->current_expr = this->builder.CreateMul(lhs, rhs); break;
		case BinaryOp::Divide: this->current_expr = this->builder.CreateSDiv(lhs, rhs); break; // TODO: lots of division ops, need to look at which one is correct
		case BinaryOp::Modulo: this->current_expr = this->builder.CreateSRem(lhs, rhs); break; // TODO: ditto
		case BinaryOp::Plus: this->current_expr = this->builder.CreateAdd(lhs, rhs); break;
		case BinaryOp::Minus: this->current_expr = this->builder.CreateSub(lhs, rhs); break;
		case BinaryOp::Less: this->current_expr = this->builder.CreateICmpSLT(lhs, rhs); break; // TODO: check this one too
		case BinaryOp::LessEqual: this->current_expr = this->builder.CreateICmpSLE(lhs, rhs); break;
		case BinaryOp::Greater: this->current_expr = this->builder.CreateICmpSGT(lhs, rhs); break;
		case BinaryOp::GreaterEqual: this->current_expr = this->builder.CreateICmpSGE(lhs, rhs); break;
		case BinaryOp::Equals: this->current_expr = this->builder.CreateICmpEQ(lhs, rhs); break;
		case BinaryOp::NotEquals: this->current_expr = this->builder.CreateICmpNE(lhs, rhs); break;
		case BinaryOp::And: this->current_expr = this->builder.CreateAnd(lhs, rhs); break;
		case BinaryOp::Or: this->current_expr = this->builder.CreateOr(lhs, rhs); break;
	}
}

void CodeGenerator::visit_assign_expr(const AssignExpr& assign_expr) {
	assign_expr.expr->accept_visitor(*this);
	llvm::Value* var = this->scope.lookup_variable(assign_expr.name);
	this->builder.CreateStore(this->current_expr, var);
}

void CodeGenerator::visit_identifier_expr(const IdentifierExpr& identifier_expr) {
	auto var = this->scope.lookup_variable(identifier_expr.name);
	this->current_expr = this->builder.CreateLoad(var);
}

void CodeGenerator::visit_func_call_expr(const FuncCallExpr& func_call_expr) {
	std::vector<llvm::Value*> params;

	for (auto& param_expr : func_call_expr.params) {
		param_expr->accept_visitor(*this);
		params.push_back(this->current_expr);
	}

	llvm::Function* func = this->module.getFunction(func_call_expr.func_name);
	this->current_expr = this->builder.CreateCall(func, params);
}

void CodeGenerator::visit_int_expr(const IntExpr& int_expr) {
	this->current_expr = llvm::ConstantInt::get(llvm::Type::getInt32Ty(this->context), int_expr.value);
}

void CodeGenerator::visit_float_expr(const FloatExpr& float_expr) {
	this->current_expr = llvm::ConstantFP::get(llvm::Type::getFloatTy(this->context), float_expr.value);
}

void CodeGenerator::visit_bool_expr(const BoolExpr& bool_expr) {
	llvm::Type* type = llvm::Type::getInt1Ty(this->context);
	this->current_expr = bool_expr.value ? llvm::ConstantInt::getTrue(type) : llvm::ConstantInt::getFalse(type);
}
