#include "codegen.hpp"
#include "ops.hpp"
#include "type_coerce.hpp"
#include "type_error.hpp"
#include <iostream>
#include <vector>

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>

//TODO functions with same name
//TODO operator associativity?
//TODO function type checking?

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


	// Register func type
	{
		std::forward_list<VarType> param_type_fl;
		for (auto& param : extern_decl.params) {
			param_type_fl.push_front(param->type);
		}
		param_type_fl.reverse();
		this->scope.register_func_type(extern_decl.name, extern_decl.return_type, param_type_fl);
	}
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

	this->scope.register_var(var_decl.name, gv, var_decl.type);
}

void CodeGenerator::visit_func_decl(const FuncDecl& func_decl) {
	// Register func type
	{
		std::forward_list<VarType> param_type_fl;
		for (auto& param : func_decl.params) {
			param_type_fl.push_front(param->type);
		}
		param_type_fl.reverse();
		this->scope.register_func_type(func_decl.name, func_decl.return_type, param_type_fl);
	}


	// CG func
	auto return_type = this->convert_return_type(func_decl.return_type);

	std::vector<llvm::Type*> param_types;
	for (auto& param : func_decl.params) {
		auto param_type = convert_var_type(param->type);
		param_types.push_back(param_type);
	}

	auto func_type = llvm::FunctionType::get(return_type, param_types, false);

	this->current_function = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, func_decl.name, this->module);

	auto body = llvm::BasicBlock::Create(this->context, func_decl.name + ":entry_point", this->current_function);
	this->builder.SetInsertPoint(body);

	// Create return block and return alloca
	this->current_return_type = func_decl.return_type;
	this->return_block = llvm::BasicBlock::Create(this->context, func_decl.name + ":return_block");
	if (func_decl.return_type != ReturnType::Void) { // If the function is non-void, make a variable to store the result
		this->return_alloca = this->builder.CreateAlloca(return_type);
		this->builder.CreateStore(llvm::Constant::getNullValue(return_type), return_alloca);
	}

	this->scope.push_scope();
	auto llvm_arg = this->current_function->arg_begin();
	for (auto& param : func_decl.params) {
		llvm_arg->setName(param->name);
		llvm::Value* alloca = this->builder.CreateAlloca(this->convert_var_type(param->type));
		this->builder.CreateStore(llvm_arg, alloca);
		this->scope.register_var(param->name, alloca, param->type);
		llvm_arg++;
	}

	this->cg_block(*func_decl.body);
	
	// Handle return block
	if (this->return_called || func_decl.return_type == ReturnType::Void) {
		return_called = false;
		this->builder.SetInsertPoint(this->return_block);
		if (func_decl.return_type == ReturnType::Void) {
			this->builder.CreateRetVoid();
		} else {
			llvm::Value* ret_val = this->builder.CreateLoad(this->return_alloca);
			this->builder.CreateRet(ret_val);
		}

		auto& block_list = this->current_function->getBasicBlockList();
		block_list.push_back(this->return_block);
	} else {
		throw std::runtime_error(std::string("semantic error: The non-void function \"") + func_decl.name + "\" does not end in a return statement.");
	}


	this->scope.pop_scope();

	llvm::verifyFunction(*this->current_function, &llvm::errs());
}

void CodeGenerator::cg_block(const Block& block) {
	for (auto& local_decl : block.var_decls) {
		this->visit_local_decl(*local_decl);
	}

	for (auto& stmt : block.statements) {
		stmt->accept_visitor(*this);
		if (this->return_called) {
			break;
		}
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
	this->scope.register_var(local_decl.name, var, local_decl.type);
}

void CodeGenerator::visit_expr_stmt(const ExprStmt& expr_stmt) {
	if (expr_stmt.expr == nullptr) return;

	expr_stmt.expr->accept_visitor(*this);
}

void CodeGenerator::visit_return_stmt(const Return& ret_stmt) {
	//TODO: typecheck here
	if (ret_stmt.return_val == nullptr) {
		if (this->current_return_type != ReturnType::Void) {
			throw TypeError(ret_stmt.line_num, ret_stmt.column_num, "return statements for non-void functions must have an associated return value");
		}
		this->builder.CreateBr(this->return_block);
	} else {
		ret_stmt.return_val->accept_visitor(*this);
		if ((size_t)this->current_return_type != (size_t)this->get_current_expr_type()) {
			throw TypeError(
				ret_stmt.line_num,
				ret_stmt.column_num,
				std::string("return value of type ")
					+ var_type_to_str(this->get_current_expr_type())
					+ " does not match the return type "
					+ return_type_to_str(this->current_return_type)
					+ " of the function"
			);
		}
		this->builder.CreateStore(this->current_expr, this->return_alloca);
		this->builder.CreateBr(this->return_block);
	}

	this->return_called = true;
}

void CodeGenerator::visit_if_else_stmt(const IfElse& if_else_stmt) {
	auto if_true_block = llvm::BasicBlock::Create(this->context, "if_true");
	auto if_false_block = llvm::BasicBlock::Create(this->context, "if_false");
	auto if_cont_block = llvm::BasicBlock::Create(this->context, "if_cont");

	// Gen condition
	if_else_stmt.cond->accept_visitor(*this);
	this->builder.CreateCondBr(this->current_expr, if_true_block, if_false_block);

	// Gen 'if_true'
	this->scope.push_scope();

	this->builder.SetInsertPoint(if_true_block);
	if_else_stmt.if_true->accept_visitor(*this);
	if (this->return_called) {
		this->return_called = false;
	} else {
		this->builder.CreateBr(if_cont_block);
	}

	this->scope.pop_scope();

	// Gen 'if_false'
	this->scope.push_scope();

	this->builder.SetInsertPoint(if_false_block);
	if (if_else_stmt.if_false != nullptr) {
		if_else_stmt.if_false->accept_visitor(*this);
	}
	if (this->return_called) {
		this->return_called = false;
	} else {
		this->builder.CreateBr(if_cont_block);
	}

	this->scope.pop_scope();

	
	// Link together
	auto& block_list = this->current_function->getBasicBlockList();
	block_list.push_back(if_true_block);
	block_list.push_back(if_false_block);
	block_list.push_back(if_cont_block);

	this->builder.SetInsertPoint(if_cont_block);
}

void CodeGenerator::visit_while_stmt(const While& while_stmt) {
	auto current_block = this->builder.GetInsertBlock();
	auto cond_check_block = llvm::BasicBlock::Create(this->context, "while_cond_check");
	auto body_block = llvm::BasicBlock::Create(this->context, "while_body");
	auto cont_block = llvm::BasicBlock::Create(this->context, "while_cont");

	// Jmp into loop
	this->builder.CreateBr(cond_check_block);

	// Gen condition
	this->builder.SetInsertPoint(cond_check_block);
	while_stmt.cond->accept_visitor(*this);
	this->builder.CreateCondBr(this->current_expr, body_block, cont_block);

	// Gen body
	this->scope.push_scope();

	this->builder.SetInsertPoint(body_block);
	while_stmt.body->accept_visitor(*this);
	if (this->return_called) {
		this->return_called = false;
	} else {
		this->builder.CreateBr(cond_check_block);
	}

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

void CodeGenerator::write_to_file(const char* filepath) {
	std::error_code ec;
	llvm::raw_fd_ostream dest(filepath, ec, llvm::sys::fs::F_None);
	if (ec) {
		std::cerr << "Failed to open file for output: " << ec.message();
		return;
	}

	this->module.print(dest, nullptr);
}


void CodeGenerator::visit_unary_expr(const UnaryExpr& unary_expr) {
	unary_expr.operand->accept_visitor(*this);

	VarType expr_type = this->get_current_expr_type();

	if (unary_expr.op == UnaryOp::Not) {
		if (expr_type == VarType::Bool) {
			this->current_expr = this->builder.CreateNot(this->current_expr);
			this->current_expr_type = VarType::Bool;
		} else {
			throw TypeError(unary_expr.line_num, unary_expr.column_num, std::string("Operand to '!' should be a bool, instead encountered a ") + var_type_to_str(expr_type));
		}
	} else if (unary_expr.op == UnaryOp::Negate) {
		std::cerr << (int)expr_type << std::endl;
		switch (expr_type) {
			case VarType::Int:
				this->current_expr = this->builder.CreateNeg(this->current_expr);
				this->current_expr_type = VarType::Int;
				break;

			case VarType::Float:
				this->current_expr = this->builder.CreateFNeg(this->current_expr);
				this->current_expr_type = VarType::Float;
				break;

			case VarType::Bool:
				throw TypeError(unary_expr.line_num, unary_expr.column_num, "Operand to '-' should be a numeric type, instead encountered a bool");
		}
	}
}

void CodeGenerator::visit_binary_expr(const BinaryExpr& binary_expr) {
	binary_expr.first_operand->accept_visitor(*this);
	llvm::Value* lhs = this->current_expr;
	VarType lhs_type = this->get_current_expr_type();

	binary_expr.second_operand->accept_visitor(*this);
	llvm::Value* rhs = this->current_expr;
	VarType rhs_type = this->get_current_expr_type();

	const OpTable& op_table = OpTable::for_op(binary_expr.op);
	std::vector<VarType> actual_operand_types { lhs_type, rhs_type };

	// Check for exact match
	for (auto& pair : op_table.entries) {
		if (pair.first.param_types == actual_operand_types) {
			this->current_expr = pair.second(this->builder, lhs, rhs);
			this->set_expr_type(pair.first.ret_type);
			return;
		}
	}

	// Check if can coerce to an available type
	for (auto& pair : op_table.entries) {
		auto maybe_coerce_funcs = coerce_list(actual_operand_types, pair.first.param_types);

		if (maybe_coerce_funcs) {
			llvm::Value* new_lhs = (*maybe_coerce_funcs)[0](this->context, this->builder, lhs);
			llvm::Value* new_rhs = (*maybe_coerce_funcs)[1](this->context, this->builder, rhs);

			this->current_expr = pair.second(this->builder, new_lhs, new_rhs);
			this->set_expr_type(pair.first.ret_type);
			return;
		}
	}

	throw TypeError(
		binary_expr.line_num,
		binary_expr.column_num,
		std::string("operator \"") + ast::expr::binary_op_symbol(binary_expr.op) + "\"",
		op_table.valid_param_types(),
		std::vector<VarType> { lhs_type, rhs_type }
	);
}

void CodeGenerator::visit_assign_expr(const AssignExpr& assign_expr) {
	assign_expr.expr->accept_visitor(*this);

	VarType actual_type = this->get_current_expr_type();
	if (auto variable_type = this->scope.lookup_variable_type(assign_expr.name)) {
		if (actual_type != *variable_type) {
			throw TypeError(
				assign_expr.line_num,
				assign_expr.column_num,
				std::string("cannot assign a value of type ") + var_type_to_str(actual_type) + " to the variable " + assign_expr.name + " of type " + var_type_to_str(*variable_type)
			);
		}
	} else {
		throw TypeError(
			assign_expr.line_num,
			assign_expr.column_num,
			std::string("in assignment, undefined variable \"") + assign_expr.name + "\""
		);
	}

	llvm::Value* var = this->scope.lookup_variable_val(assign_expr.name);
	this->builder.CreateStore(this->current_expr, var);
}

void CodeGenerator::visit_identifier_expr(const IdentifierExpr& identifier_expr) {
	auto var = this->scope.lookup_variable_val(identifier_expr.name);
	if (var == nullptr) {
		throw TypeError(
			identifier_expr.line_num,
			identifier_expr.column_num,
			std::string("undefined variable \"") + identifier_expr.name + "\""
		);
	}
	this->current_expr = this->builder.CreateLoad(var);
	this->current_expr_type = this->scope.lookup_variable_type(identifier_expr.name);
}

void CodeGenerator::visit_func_call_expr(const FuncCallExpr& func_call_expr) {
	if (auto func_type = this->scope.lookup_func_type(func_call_expr.func_name)) {
		auto func_ret_type = func_type->first;
		auto func_param_types = func_type->second;

		std::vector<llvm::Value*> params;

		for (auto& param_expr : func_call_expr.params) {
			param_expr->accept_visitor(*this);
			params.push_back(this->current_expr);
		}

		llvm::Function* func = this->module.getFunction(func_call_expr.func_name);
		this->current_expr = this->builder.CreateCall(func, params);

		switch (func_ret_type) {
			case ReturnType::Int:
				this->current_expr_type = VarType::Int;
				break;

			case ReturnType::Float:
				this->current_expr_type = VarType::Float;
				break;

			case ReturnType::Bool:
				this->current_expr_type = VarType::Bool;
				break;

			case ReturnType::Void:
				this->current_expr_type = boost::none;
				break;
		}
	} else {
		throw TypeError(
			func_call_expr.line_num,
			func_call_expr.column_num,
			std::string("undefined function \"") + func_call_expr.func_name + "\""
		);
	}
}

void CodeGenerator::visit_int_expr(const IntExpr& int_expr) {
	this->current_expr = llvm::ConstantInt::get(llvm::Type::getInt32Ty(this->context), int_expr.value);
	this->current_expr_type = VarType::Int;
}

void CodeGenerator::visit_float_expr(const FloatExpr& float_expr) {
	this->current_expr = llvm::ConstantFP::get(llvm::Type::getFloatTy(this->context), float_expr.value);
	this->current_expr_type = VarType::Float;
}

void CodeGenerator::visit_bool_expr(const BoolExpr& bool_expr) {
	llvm::Type* type = llvm::Type::getInt1Ty(this->context);
	this->current_expr = bool_expr.value ? llvm::ConstantInt::getTrue(type) : llvm::ConstantInt::getFalse(type);
	this->current_expr_type = VarType::Bool;
}

VarType CodeGenerator::get_current_expr_type() {
	if (this->current_expr_type) {
		return *this->current_expr_type;
	}

	throw std::runtime_error("Cannot operate on something of type void");
}

void CodeGenerator::set_expr_type(ReturnType ret_type) {
	switch (ret_type) {
		case ReturnType::Void: this->current_expr_type = boost::none; break;
		case ReturnType::Int: this->current_expr_type = VarType::Int; break;
		case ReturnType::Float: this->current_expr_type = VarType::Float; break;
		case ReturnType::Bool: this->current_expr_type = VarType::Bool; break;
	}
}

void CodeGenerator::assert_type_eq(VarType expected, VarType received) {
	if (expected != received) {
		std::string error_str = "Expected: ";
		error_str += var_type_to_str(expected);
		error_str += ", Got: ";
		error_str += var_type_to_str(received);
		throw std::runtime_error(error_str);
	}
}
