#include "parse.hpp"
#include "token_stream.hpp"
#include "../ast/expr.hpp"
#include "../ast/statement.hpp"
#include "../ast/declaration.hpp"
#include "parse_error.hpp"
#include <iostream>
#include <forward_list>
#include <memory>
#include <llvm/ADT/STLExtras.h>

using namespace ast::expr;
using namespace ast::statement;
using namespace ast::declaration;

Parser::Parser(TokenStream& ts) noexcept :
	ts(ts) { }


std::unique_ptr<Program> Parser::parse_program() {
	// program ::= extern_list decl_list
	
	auto program = llvm::make_unique<Program>();

	program->externs = this->parse_extern_list();
	program->decls = this->parse_decl_list();

	this->consume(Token::Type::EndOfInput);

	return program;
}

std::forward_list<std::unique_ptr<Declaration>> Parser::parse_decl_list() {
	auto decl = this->parse_decl();

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
		case Token::Type::Void:
			{
				auto decl_list = this->parse_decl_list();
				decl_list.push_front(std::move(decl));
				return decl_list;
			}

		default:
			throw ParseError(
				this->ts.current_line(),
				this->ts.current_column(),
				"a list of declarations",
				"the beginning of a variable declaration, function declaration or end of file",
				std::vector<Token::Type> {
					Token::Type::EndOfInput,
					Token::Type::Int,
					Token::Type::Float,
					Token::Type::Bool,
					Token::Type::Void
				},
				ts.next()
			);
	}
}

std::unique_ptr<Declaration> Parser::parse_decl() {
	if (ts.peek_type(1) == Token::Type::Void) {
		// parse a void function
		auto decl = llvm::make_unique<FuncDecl>();

		decl->return_type = this->parse_return_type();
		decl->name = this->parse_identifier();
		this->consume(Token::Type::LParen);
		decl->params = this->parse_params();
		this->consume(Token::Type::RParen);
		decl->body = this->parse_block();

		return std::move(decl);
	}

	// parse a non-void function
	auto var_type = this->parse_var_type();
	auto name = this->parse_identifier();

	const Token& symbol = ts.next();
	switch (symbol.type) {
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
				func_decl->params = this->parse_params();
				func_decl->name = name;
				this->consume(Token::Type::RParen);
				func_decl->body = this->parse_block();

				return std::move(func_decl);
			}

		default:
			throw ParseError(
				this->ts.current_line(),
				this->ts.current_column(),
				"a variable or function declaration",
				"parameter list or end of variable declaration",
				std::vector<Token::Type> {
					Token::Type::SemiColon,
					Token::Type::LParen
				},
				symbol
			);
	}
}

std::unique_ptr<Block> Parser::parse_block() {
	this->consume(Token::Type::LBrace);

	auto block = llvm::make_unique<Block>();
	block->var_decls = this->parse_local_decl_list();
	block->statements = this->parse_stmt_list();

	this->consume(Token::Type::RBrace);

	return block;
}

std::forward_list<std::unique_ptr<VarDecl>> Parser::parse_local_decl_list() {
	switch (ts.peek_type(1)) {
		// local_decls ::= local_decl local_decls
		case Token::Type::Int:
		case Token::Type::Float:
		case Token::Type::Bool:
			{
				auto local_decl = this->parse_local_decl();
				auto local_decl_list = this->parse_local_decl_list();

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
			throw ParseError(
				this->ts.current_line(),
				this->ts.current_column(),
				"a local variable declaration list",
				"another variable declaration or start of statement list",
				std::vector<Token::Type> {
					Token::Type::Int,
					Token::Type::Float,
					Token::Type::Bool,
					Token::Type::While,
					Token::Type::If,
					Token::Type::LBrace,
					Token::Type::Return,
					Token::Type::SemiColon,
					Token::Type::Minus,
					Token::Type::Not,
					Token::Type::LParen,
					Token::Type::Identifier,
					Token::Type::IntLit,
					Token::Type::FloatLit,
					Token::Type::BoolLit,
					Token::Type::RBrace
				},
				this->ts.next()
			);
	}
}

std::unique_ptr<VarDecl> Parser::parse_local_decl() {
	auto local_decl = llvm::make_unique<VarDecl>();
	local_decl->type = this->parse_var_type();
	local_decl->name = this->parse_identifier();
	this->consume(Token::Type::SemiColon);
	return local_decl;
}

std::forward_list<std::unique_ptr<Statement>> Parser::parse_stmt_list() {
	if (ts.peek_type(1) == Token::Type::RBrace) {
		std::forward_list<std::unique_ptr<Statement>> stmt_list;
		return stmt_list;
	}

	auto stmt = this->parse_stmt();
	auto stmt_list = this->parse_stmt_list();
	stmt_list.push_front(std::move(stmt));
	return stmt_list;
}

std::unique_ptr<Statement> Parser::parse_stmt() {
	switch (ts.peek_type(1)) {
		case Token::Type::While: return this->parse_while_stmt();
		case Token::Type::If: return this->parse_if_stmt();
		case Token::Type::Return: return this->parse_return_stmt();
		case Token::Type::LBrace: return this->parse_block();
		default: return this->parse_expr_stmt();
	}
}

std::unique_ptr<Statement> Parser::parse_expr_stmt() {
	auto stmt = llvm::make_unique<ExprStmt>();

	if (ts.peek_type(1) == Token::Type::SemiColon) {
		this->consume(Token::Type::SemiColon);
		stmt->expr = nullptr;
	} else {
		stmt->expr = this->parse_expr();
		this->consume(Token::Type::SemiColon);
	}

	return stmt;
}

std::unique_ptr<Statement> Parser::parse_if_stmt() {
	auto stmt = llvm::make_unique<IfElse>();

	this->consume(Token::Type::If);
	this->consume(Token::Type::LParen);
	stmt->cond = this->parse_expr();
	this->consume(Token::Type::RParen);
	stmt->if_true = this->parse_block();

	if (ts.peek_type(1) == Token::Type::Else) {
		this->consume(Token::Type::Else);
		stmt->if_false = this->parse_block();
	}

	return stmt;
}

std::unique_ptr<Statement> Parser::parse_while_stmt() {
	auto stmt = llvm::make_unique<While>();

	this->consume(Token::Type::While);
	this->consume(Token::Type::LParen);
	stmt->cond = this->parse_expr();
	this->consume(Token::Type::RParen);
	stmt->body = this->parse_stmt();

	return stmt;
}

std::unique_ptr<Statement> Parser::parse_return_stmt() {
	auto stmt = llvm::make_unique<Return>();

	this->consume(Token::Type::Return);

	if (ts.peek_type(1) != Token::Type::SemiColon) {
		stmt->return_val = this->parse_expr();
	}

	return stmt;
}

std::forward_list<std::unique_ptr<ExternDecl>> Parser::parse_extern_list() {
	switch (ts.peek_type(1)) {
		// program ::= extern extern_list
		case Token::Type::Extern:
			{
				auto extern_decl = this->parse_extern();
				auto extern_list = this->parse_extern_list();

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
			throw ParseError("Expected token of type Extern, Void, Int, Float or Bool!");
	}
}

std::unique_ptr<ExternDecl> Parser::parse_extern() {
	// extern ::= "extern" return_type IDENTIFIER "(" params ")" ";"
	ts.next();

	auto extern_decl = llvm::make_unique<ExternDecl>();

	extern_decl->return_type = this->parse_return_type();
	extern_decl->name = this->parse_identifier();
	this->consume(Token::Type::LParen);
	extern_decl->params = this->parse_params();
	this->consume(Token::Type::RParen);
	this->consume(Token::Type::SemiColon);

	return extern_decl;
}

std::forward_list<std::unique_ptr<Param>> Parser::parse_params() {
	switch (ts.peek_type(1)) {
		// params ::= ε
		case Token::Type::RParen: 
			return std::forward_list<std::unique_ptr<Param>>();

		// params ::= "void"
		case Token::Type::Void:
			this->ts.next();
			return std::forward_list<std::unique_ptr<Param>>();

		case Token::Type::Int:
		case Token::Type::Float:
		case Token::Type::Bool:
			return this->parse_param_list();

		default:
			throw ParseError("Expected list of params, or \"void\", found something else...");
	}
}

std::forward_list<std::unique_ptr<Param>> Parser::parse_param_list() {
	auto param = this->parse_param();

	if (ts.peek_type(1) == Token::Type::Comma) {
		this->ts.next();
		auto param_list = this->parse_param_list();
		param_list.push_front(std::move(param));
		return param_list;
	} else {
		std::forward_list<std::unique_ptr<Param>> param_list;
		param_list.push_front(std::move(param));
		return param_list;
	}
}

std::unique_ptr<Param> Parser::parse_param() {
	auto param = llvm::make_unique<Param>();

	param->type = this->parse_var_type();
	param->name = this->parse_identifier();

	return param;
}

std::string Parser::parse_identifier() {
	const Token& t = this->ts.next();

	if (t.type == Token::Type::Identifier) {
		return std::string(t.lexeme);
	}

	throw ParseError("Expected identifier, found something else...");
}

VarType Parser::parse_var_type() {
	switch (ts.next().type) {
		// var_type ::= "int"
		case Token::Type::Int: return VarType::Int;

		// var_type ::= "float"
		case Token::Type::Float: return VarType::Float;

		// var_type ::= "bool"
		case Token::Type::Bool: return VarType::Bool;

		default: throw ParseError("Expected variable type, found something else...");
	}
}

ReturnType Parser::parse_return_type() {
	switch (ts.next().type) {
		// return_type ::= "void"
		case Token::Type::Void: return ReturnType::Void;

		// return_type ::= "int"
		case Token::Type::Int: return ReturnType::Int;

		// return_type ::= "float"
		case Token::Type::Float: return ReturnType::Float;

		// return_type ::= "bool"
		case Token::Type::Bool: return ReturnType::Bool;

		default: throw ParseError("Expected return type, found something else...");
	}
}

std::unique_ptr<Expr> Parser::parse_expr() {
	auto lhs = this->parse_primary_expr();
	return this->parse_bin_op(std::move(lhs), 0);
}

std::unique_ptr<Expr> Parser::parse_bin_op(std::unique_ptr<Expr> lhs, unsigned int min_precedence) {
	while (true) {
		if (auto op = binary_op_from_token_type(ts.peek_type(1))) {
			if (binary_op_precedence(*op) < min_precedence) {
				return lhs;
			}
			this->ts.next();

			auto rhs = this->parse_primary_expr();

			if (auto next_op = binary_op_from_token_type(ts.peek_type(1))) {
				if (binary_op_precedence(*op) < binary_op_precedence(*next_op)) {
					rhs = this->parse_bin_op(std::move(rhs), min_precedence + 1);
				}

				lhs = llvm::make_unique<BinaryExpr>(*op, std::move(lhs), std::move(rhs));

			} else {
				lhs = llvm::make_unique<BinaryExpr>(*op, std::move(lhs), std::move(rhs));
			}


		} else {
			return lhs;
		}
	}
}

std::unique_ptr<Expr> Parser::parse_primary_expr() {
	switch (ts.peek_type(1)) {
		case Token::Type::Not: return this->parse_not_expr();
		case Token::Type::Minus: return this->parse_negate_expr();
		case Token::Type::IntLit: return this->parse_int_expr();
		case Token::Type::FloatLit: return this->parse_float_expr();
		case Token::Type::BoolLit: return this->parse_bool_expr();
		case Token::Type::LParen: return this->parse_parens_expr();
		case Token::Type::Identifier: {
				switch (ts.peek_type(2)) {
					case Token::Type::Assign: return this->parse_assign_expr();
					case Token::Type::LParen: return this->parse_func_call_expr();
					default: return this->parse_identifier_expr();
				}
			}
		default: throw ParseError("Expected beginning of expression, found sometihng else");
	}
}

std::unique_ptr<Expr> Parser::parse_not_expr() {
	this->consume(Token::Type::Not);
	//auto expr = parse_expr(ts);
	auto expr = this->parse_primary_expr();
	return llvm::make_unique<UnaryExpr>(UnaryOp::Not, std::move(expr));
}

std::unique_ptr<Expr> Parser::parse_negate_expr() {
	this->consume(Token::Type::Minus);
	//auto expr = parse_expr(ts);
	auto expr = this->parse_primary_expr();
	return llvm::make_unique<UnaryExpr>(UnaryOp::Negate, std::move(expr));
}

std::unique_ptr<Expr> Parser::parse_int_expr() {
	// TODO: does this int parsing function meet the parsing spec?
	
	int value = std::stoi(std::string(this->ts.next().lexeme));
	return llvm::make_unique<IntExpr>(value);
}

std::unique_ptr<Expr> Parser::parse_float_expr() {
	float value = std::stof(std::string(this->ts.next().lexeme));
	return llvm::make_unique<FloatExpr>(value);
}

std::unique_ptr<Expr> Parser::parse_bool_expr() {
	auto s = this->ts.next().lexeme;
	
	if (s == boost::string_ref("true")) {
		return llvm::make_unique<BoolExpr>(true);
	} else {
		return llvm::make_unique<BoolExpr>(false);
	}
}

std::unique_ptr<Expr> Parser::parse_parens_expr() {
	this->consume(Token::Type::LParen);
	auto expr = this->parse_expr();
	this->consume(Token::Type::RParen);
	return expr;
}

std::unique_ptr<Expr> Parser::parse_assign_expr() {
	auto assign_expr = llvm::make_unique<AssignExpr>();

	assign_expr->name = this->parse_identifier();
	this->consume(Token::Type::Assign);
	assign_expr->expr = this->parse_expr();

	return assign_expr;
}

std::unique_ptr<Expr> Parser::parse_func_call_expr() {
	auto fc_expr = llvm::make_unique<FuncCallExpr>();

	fc_expr->func_name = this->parse_identifier();
	this->consume(Token::Type::LParen);
	fc_expr->params = this->parse_args();
	this->consume(Token::Type::RParen);

	return fc_expr;
}

std::unique_ptr<Expr> Parser::parse_identifier_expr() {
	return llvm::make_unique<IdentifierExpr>(std::string(this->ts.next().lexeme));
}

std::forward_list<std::unique_ptr<Expr>> Parser::parse_args() {
	switch (this->ts.peek_type(1)) {
		case Token::Type::RBrace: {
				std::forward_list<std::unique_ptr<Expr>> args;
				return args;
			}
		default: return this->parse_arg_list();
	}
}

std::forward_list<std::unique_ptr<Expr>> Parser::parse_arg_list() {
	auto expr = this->parse_expr();
	
	if (this->ts.peek_type(1) == Token::Type::Comma) {
		this->ts.next();
		auto arg_list = this->parse_arg_list();
		arg_list.push_front(std::move(expr));
		return arg_list;
	}

	std::forward_list<std::unique_ptr<Expr>> args;
	args.push_front(std::move(expr));
	return args;
}

void Parser::consume(Token::Type expected_type) {
	Token::Type actual_type = this->ts.next().type;
	if (actual_type != expected_type) {
		throw ParseError(std::string("Expected token of type ") + Token::type_to_str(expected_type) + ", instead encountered " + Token::type_to_str(actual_type));
	}
}
