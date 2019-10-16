#include "parse.hpp"
#include "token_stream.hpp"
#include "../ast/expr.hpp"
#include "../ast/statement.hpp"
#include "../ast/declaration.hpp"
#include "parse_exception.hpp"
#include <iostream>
#include <forward_list>

using namespace ast::expr;
using namespace ast::statement;
using namespace ast::declaration;

std::unique_ptr<Program> parse_program(TokenStream& ts) {
	// program ::= extern_list decl_list
	
	auto program = std::make_unique<Program>();

	program->externs = parse_extern_list(ts);
	program->decls = parse_decl_list(ts);

	return std::move(program);
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
		auto decl = std::make_unique<FuncDecl>();

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
				auto var_decl = std::make_unique<VarDecl>();

				var_decl->type = var_type;
				var_decl->name = name;

				return std::move(var_decl);
			}

		case Token::Type::LParen:
			{
				auto func_decl = std::make_unique<FuncDecl>();

				func_decl->return_type = static_cast<ReturnType>(var_type);
				func_decl->params = parse_params(ts);
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
	consume(ts, Token::Type::RBrace);

	return nullptr;
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

	auto extern_decl = std::make_unique<ExternDecl>();

	extern_decl->return_type = parse_return_type(ts);
	extern_decl->name = parse_identifier(ts);
	consume(ts, Token::Type::LParen);
	extern_decl->params = parse_params(ts);
	consume(ts, Token::Type::RParen);
	consume(ts, Token::Type::SemiColon);

	return std::move(extern_decl);
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
	auto param = std::make_unique<Param>();

	param->type = parse_var_type(ts);
	param->name = parse_identifier(ts);

	return std::move(param);
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

void consume(TokenStream& ts, Token::Type expected_type) {
	Token::Type actual_type = ts.next().type;
	if (actual_type != expected_type) {
		throw ParseException(std::string("Expected token of type ") + Token::type_to_str(expected_type) + ", instead encountered " + Token::type_to_str(actual_type));
	}
}
