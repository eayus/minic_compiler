#include "parse.hpp"
#include "token_stream.hpp"
#include "../ast/expr.hpp"
#include "../ast/statement.hpp"
#include "../ast/declaration.hpp"
#include "parse_exception.hpp"
#include <iostream>
#include <forward_list>
#include <memory>
#include <llvm/ADT/STLExtras.h>

using namespace ast::expr;
using namespace ast::statement;
using namespace ast::declaration;

std::unique_ptr<Program> parse_program(TokenStream& ts) {
	// program ::= extern_list decl_list
	
	auto program = llvm::make_unique<Program>();

	program->externs = parse_extern_list(ts);
	program->decls = parse_decl_list(ts);

	return program;
}

std::forward_list<std::unique_ptr<Declaration>> parse_decl_list(TokenStream& ts) {
	auto decl = parse_decl(ts);

	switch (ts.peek_type(1)) {
		// decl_list ::= decl
		case Token::Type::EndOfInput:
			{
				std::forward_list<std::unique_ptr<Declaration>> decl_list;
				decl_list.push_front(std::move(decl));
				return decl_list;
			}
			//return std::forward_list<std::unique_ptr<Declaration>> { std::move(decl) };

		// decl_list ::= decl decl_list
		case Token::Type::Int:
		case Token::Type::Float:
		case Token::Type::Bool:
			{
				auto decl_list = parse_decl_list(ts);
				decl_list.push_front(std::move(decl));
				return decl_list;
			}

		default:
			throw ParseException("Expected another variable/function declaration or EOF!");
	}
}

std::unique_ptr<Declaration> parse_decl(TokenStream& ts) {
	if (ts.peek_type(1) == Token::Type::Void) {
		// parse a void function
		auto decl = llvm::make_unique<FuncDecl>();

		decl->return_type = parse_return_type(ts);
		decl->name = parse_identifier(ts);
		consume(ts, Token::Type::LParen);
		decl->params = parse_params(ts);
		consume(ts, Token::Type::RParen);
		decl->body = parse_block(ts);

		return std::move(decl);
	}

	// parse a non-void function
	auto var_type = parse_var_type(ts);
	auto name = parse_identifier(ts);

	switch (ts.next().type) {
		case Token::Type::SemiColon:
			{
				auto var_decl = llvm::make_unique<VarDecl>();

				var_decl->type = var_type;
				var_decl->name = name;

				return std::move(var_decl);
			}

		case Token::Type::LParen:
			{
				auto func_decl = llvm::make_unique<FuncDecl>();

				func_decl->return_type = static_cast<ReturnType>(var_type);
				func_decl->params = parse_params(ts);
				func_decl->name = name;
				consume(ts, Token::Type::RParen);
				func_decl->body = parse_block(ts);

				return std::move(func_decl);
			}

		default:
			throw ParseException("Expected either a semicolon (for var decl) or left paren (for func decl)");
	}
}

std::unique_ptr<Block> parse_block(TokenStream& ts) {
	consume(ts, Token::Type::LBrace);

	auto block = llvm::make_unique<Block>();
	block->var_decls = parse_local_decl_list(ts);
	block->statements = parse_stmt_list(ts);

	consume(ts, Token::Type::RBrace);

	return block;
}

std::forward_list<std::unique_ptr<VarDecl>> parse_local_decl_list(TokenStream& ts) {
	switch (ts.peek_type(1)) {
		// local_decls ::= local_decl local_decls
		case Token::Type::Int:
		case Token::Type::Float:
		case Token::Type::Bool:
			{
				auto local_decl = parse_local_decl(ts);
				auto local_decl_list = parse_local_decl_list(ts);

				local_decl_list.push_front(std::move(local_decl));

				return local_decl_list;
			}

		// local_decls ::= epsilon
		case Token::Type::While:
		case Token::Type::If:
		case Token::Type::LBrace:
		case Token::Type::Return:
		case Token::Type::SemiColon:
		case Token::Type::Minus:
		case Token::Type::Not:
		case Token::Type::LParen:
		case Token::Type::Identifier:
		case Token::Type::IntLit:
		case Token::Type::FloatLit:
		case Token::Type::BoolLit:
		case Token::Type::RBrace:
			{
				std::forward_list<std::unique_ptr<VarDecl>> var_decl_list;
				return var_decl_list;
			}

		default:
			throw ParseException("Expected beginning of a variable declaration, or statement!");
	}
}

std::unique_ptr<VarDecl> parse_local_decl(TokenStream& ts) {
	auto local_decl = llvm::make_unique<VarDecl>();
	local_decl->type = parse_var_type(ts);
	local_decl->name = parse_identifier(ts);
	consume(ts, Token::Type::SemiColon);
	return local_decl;
}

std::forward_list<std::unique_ptr<Statement>> parse_stmt_list(TokenStream& ts) {
	if (ts.peek_type(1) == Token::Type::RBrace) {
		std::forward_list<std::unique_ptr<Statement>> stmt_list;
		return stmt_list;
	}

	auto stmt = parse_stmt(ts);
	auto stmt_list = parse_stmt_list(ts);
	stmt_list.push_front(std::move(stmt));
	return stmt_list;
}

std::unique_ptr<Statement> parse_stmt(TokenStream& ts) {
	switch (ts.peek_type(1)) {
		case Token::Type::While: return parse_while_stmt(ts);
		case Token::Type::If: return parse_if_stmt(ts);
		case Token::Type::Return: return parse_return_stmt(ts);
		case Token::Type::LBrace: return parse_block(ts);
		default: return parse_expr_stmt(ts);
	}
}

std::unique_ptr<Statement> parse_expr_stmt(TokenStream& ts) {
	auto stmt = llvm::make_unique<ExprStmt>();

	if (ts.peek_type(1) == Token::Type::SemiColon) {
		consume(ts, Token::Type::SemiColon);
		stmt->expr = nullptr;
	} else {
		stmt->expr = parse_expr(ts);
		consume(ts, Token::Type::SemiColon);
	}

	return stmt;
}

std::unique_ptr<Statement> parse_if_stmt(TokenStream& ts) {
	auto stmt = llvm::make_unique<IfElse>();

	consume(ts, Token::Type::If);
	consume(ts, Token::Type::LParen);
	stmt->cond = parse_expr(ts);
	consume(ts, Token::Type::RParen);
	stmt->if_true = parse_block(ts);

	if (ts.peek_type(1) == Token::Type::Else) {
		consume(ts, Token::Type::Else);
		stmt->if_false = parse_block(ts);
	}

	return stmt;
}

std::unique_ptr<Statement> parse_while_stmt(TokenStream& ts) {
	auto stmt = llvm::make_unique<While>();

	consume(ts, Token::Type::While);
	consume(ts, Token::Type::LParen);
	stmt->cond = parse_expr(ts);
	consume(ts, Token::Type::RParen);
	stmt->body = parse_stmt(ts);

	return stmt;
}

std::unique_ptr<Statement> parse_return_stmt(TokenStream& ts) {
	auto stmt = llvm::make_unique<Return>();

	consume(ts, Token::Type::Return);

	if (ts.peek_type(1) != Token::Type::SemiColon) {
		stmt->return_val = parse_expr(ts);
	}

	return stmt;
}

std::forward_list<std::unique_ptr<ExternDecl>> parse_extern_list(TokenStream& ts) {
	switch (ts.peek_type(1)) {
		// program ::= extern extern_list
		case Token::Type::Extern:
			{
				auto extern_decl = parse_extern(ts);
				auto extern_list = parse_extern_list(ts);

				extern_list.push_front(std::move(extern_decl));
				return extern_list;
			}

		// program ::= epsilon
		case Token::Type::Void:
		case Token::Type::Int:
		case Token::Type::Float:
		case Token::Type::Bool:
			{
				std::forward_list<std::unique_ptr<ExternDecl>> extern_list;
				return extern_list;
			}

		default:
			throw ParseException("Expected token of type Extern, Void, Int, Float or Bool!");
	}
}

std::unique_ptr<ExternDecl> parse_extern(TokenStream& ts) {
	// extern ::= "extern" return_type IDENTIFIER "(" params ")" ";"
	ts.next();

	auto extern_decl = llvm::make_unique<ExternDecl>();

	extern_decl->return_type = parse_return_type(ts);
	extern_decl->name = parse_identifier(ts);
	consume(ts, Token::Type::LParen);
	extern_decl->params = parse_params(ts);
	consume(ts, Token::Type::RParen);
	consume(ts, Token::Type::SemiColon);

	return extern_decl;
}

std::forward_list<std::unique_ptr<Param>> parse_params(TokenStream& ts) {
	switch (ts.peek_type(1)) {
		// params ::= ε
		case Token::Type::RParen: 
			return std::forward_list<std::unique_ptr<Param>>();

		// params ::= "void"
		case Token::Type::Void:
			ts.next();
			return std::forward_list<std::unique_ptr<Param>>();

		case Token::Type::Int:
		case Token::Type::Float:
		case Token::Type::Bool:
			return parse_param_list(ts);

		default:
			throw ParseException("Expected list of params, or \"void\", found something else...");
	}
}

std::forward_list<std::unique_ptr<Param>> parse_param_list(TokenStream& ts) {
	auto param = parse_param(ts);

	if (ts.peek_type(1) == Token::Type::Comma) {
		ts.next();
		auto param_list = parse_param_list(ts);
		param_list.push_front(std::move(param));
		return param_list;
	} else {
		std::forward_list<std::unique_ptr<Param>> param_list;
		param_list.push_front(std::move(param));
		return param_list;
	}
}

std::unique_ptr<Param> parse_param(TokenStream& ts) {
	auto param = llvm::make_unique<Param>();

	param->type = parse_var_type(ts);
	param->name = parse_identifier(ts);

	return param;
}

std::string parse_identifier(TokenStream& ts) {
	const Token& t = ts.next();

	if (t.type == Token::Type::Identifier) {
		return std::string(t.lexeme);
	}

	throw ParseException("Expected identifier, found something else...");
}

VarType parse_var_type(TokenStream& ts) {
	switch (ts.next().type) {
		// var_type ::= "int"
		case Token::Type::Int: return VarType::Int;

		// var_type ::= "float"
		case Token::Type::Float: return VarType::Float;

		// var_type ::= "bool"
		case Token::Type::Bool: return VarType::Bool;

		default: throw ParseException("Expected variable type, found something else...");
	}
}

ReturnType parse_return_type(TokenStream& ts) {
	switch (ts.next().type) {
		// return_type ::= "void"
		case Token::Type::Void: return ReturnType::Void;

		// return_type ::= "int"
		case Token::Type::Int: return ReturnType::Int;

		// return_type ::= "float"
		case Token::Type::Float: return ReturnType::Float;

		// return_type ::= "bool"
		case Token::Type::Bool: return ReturnType::Bool;

		default: throw ParseException("Expected return type, found something else...");
	}
}

std::unique_ptr<Expr> parse_expr(TokenStream& ts) {
	auto lhs = parse_primary_expr(ts);
	return parse_bin_op(ts, std::move(lhs), 0);
}

std::unique_ptr<Expr> parse_bin_op(TokenStream& ts, std::unique_ptr<Expr> lhs, unsigned int min_precedence) {
	while (true) {
		if (auto op = binary_op_from_token_type(ts.peek_type(1))) {
			if (binary_op_precedence(*op) < min_precedence) {
				return lhs;
			}
			ts.next();

			auto rhs = parse_primary_expr(ts);

			if (auto next_op = binary_op_from_token_type(ts.peek_type(1))) {
				if (binary_op_precedence(*op) < binary_op_precedence(*next_op)) {
					return parse_bin_op(ts, std::move(rhs), min_precedence + 1);
				}

				lhs = llvm::make_unique<BinaryExpr>(*op, std::move(lhs), std::move(rhs));

			} else {
				return llvm::make_unique<BinaryExpr>(*op, std::move(lhs), std::move(rhs));
			}


		} else {
			return lhs;
		}
	}
}

std::unique_ptr<Expr> parse_primary_expr(TokenStream& ts) {
	switch (ts.peek_type(1)) {
		case Token::Type::Not: return parse_not_expr(ts);
		case Token::Type::Minus: return parse_negate_expr(ts);
		case Token::Type::IntLit: return parse_int_expr(ts);
		case Token::Type::FloatLit: return parse_float_expr(ts);
		case Token::Type::BoolLit: return parse_bool_expr(ts);
		case Token::Type::LParen: return parse_parens_expr(ts);
		case Token::Type::Identifier: {
				switch (ts.peek_type(2)) {
					case Token::Type::Assign: return parse_assign_expr(ts);
					case Token::Type::LParen: return parse_func_call_expr(ts);
					default: return parse_identifier_expr(ts);
				}
			}
		default: throw ParseException("Expected beginning of expression, found sometihng else");
	}
}

std::unique_ptr<Expr> parse_not_expr(TokenStream& ts) {
	consume(ts, Token::Type::Not);
	return parse_expr(ts);
}

std::unique_ptr<Expr> parse_negate_expr(TokenStream& ts) {
	consume(ts, Token::Type::Minus);
	return parse_expr(ts);
}

std::unique_ptr<Expr> parse_int_expr(TokenStream& ts) {
	// TODO: does this int parsing function meet the parsing spec?
	
	int value = std::stoi(std::string(ts.next().lexeme));
	return llvm::make_unique<IntExpr>(value);
}

std::unique_ptr<Expr> parse_float_expr(TokenStream& ts) {
	float value = std::stof(std::string(ts.next().lexeme));
	return llvm::make_unique<FloatExpr>(value);
}

std::unique_ptr<Expr> parse_bool_expr(TokenStream& ts) {
	auto s = ts.next().lexeme;
	
	if (s == boost::string_ref("true")) {
		return llvm::make_unique<BoolExpr>(true);
	} else {
		return llvm::make_unique<BoolExpr>(false);
	}
}

std::unique_ptr<Expr> parse_parens_expr(TokenStream& ts) {
	consume(ts, Token::Type::LParen);
	auto expr = parse_expr(ts);
	consume(ts, Token::Type::RParen);
	return expr;
}

std::unique_ptr<Expr> parse_assign_expr(TokenStream& ts) {
	auto assign_expr = llvm::make_unique<AssignExpr>();

	assign_expr->name = parse_identifier(ts);
	consume(ts, Token::Type::Assign);
	assign_expr->expr = parse_expr(ts);

	return assign_expr;
}

std::unique_ptr<Expr> parse_func_call_expr(TokenStream& ts) {
	auto fc_expr = llvm::make_unique<FuncCallExpr>();

	fc_expr->func_name = parse_identifier(ts);
	consume(ts, Token::Type::LParen);
	fc_expr->params = parse_args(ts);
	consume(ts, Token::Type::RParen);

	return fc_expr;
}

std::unique_ptr<Expr> parse_identifier_expr(TokenStream& ts) {
	return llvm::make_unique<IdentifierExpr>(std::string(ts.next().lexeme));
}

std::forward_list<std::unique_ptr<Expr>> parse_args(TokenStream& ts) {
	switch (ts.peek_type(1)) {
		case Token::Type::RBrace: {
				std::forward_list<std::unique_ptr<Expr>> args;
				return args;
			}
		default: return parse_arg_list(ts);
	}
}

std::forward_list<std::unique_ptr<Expr>> parse_arg_list(TokenStream& ts) {
	auto expr = parse_expr(ts);
	
	if (ts.peek_type(1) == Token::Type::Comma) {
		ts.next();
		auto arg_list = parse_arg_list(ts);
		arg_list.push_front(std::move(expr));
		return arg_list;
	}

	std::forward_list<std::unique_ptr<Expr>> args;
	args.push_front(std::move(expr));
	return args;
}

void consume(TokenStream& ts, Token::Type expected_type) {
	Token::Type actual_type = ts.next().type;
	if (actual_type != expected_type) {
		throw ParseException(std::string("Expected token of type ") + Token::type_to_str(expected_type) + ", instead encountered " + Token::type_to_str(actual_type));
	}
}
