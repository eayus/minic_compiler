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
	
	std::unique_ptr<Program> program;

	program.externs = parse_extern_list(ts);
	program.decls = parse_decl_list(ts);

	return std::move(program);
}

std::forward_list<std::unique_ptr<ExternDecl>> parse_extern_list(TokenStream& ts) {
	switch (ts.peek_type(1)) {
		// program ::= extern extern_list
		case Token::Type::Extern:
			auto extern_decl = parse_extern(ts);
			auto extern_list = parse_extern_list(ts);

			extern_list.push_front(std::move(extern_decl));
			return extern_list;

		// program ::= epsilon
		case Token::Type::Void:
		case Token::Type::Int:
		case Token::Type::Float:
		case Token::Type::Bool:
			return std::forward_list<std::unique_ptr<ExternDecl>>;

		default:
			throw ParseException("Expected token of type Extern, Void, Int, Float or Bool!");
	}
}

std::unique_ptr<ExternDecl> parse_extern(TokenStream& ts) {
	// extern ::= "extern" return_type IDENTIFIER "(" params ")" ";"
	
	ts.next();

	std::unique_ptr<ExternDecl> extern_decl;

	extern_decl.return_type = parse_return_type(ts);
	extern_decl.name = parse_identifier(ts);
	consume(ts, Token::Type::LParen);
	extern_decl.params = parse_params(ts);
	consume(ts, Token::Type::RParen);
	consume(ts, Token::Type::SemiColon);

	return std::move(extern_decl);
}

std::forward_list<std::unique_ptr<Param>> parse_params(TokenStream& ts) {
	switch (ts.peek_type(1)) {
		// params ::= ε | "void"
		case Token::Type::RParen: 
		case Token::Type::Void:
			return std::forward_list<std::unique_ptr<Param>>;

		case Token::Type::VarType:
			return parse_param_list(ts);

		default:
			throw ParseException("Expected list of params, or "void", found something else...");
	}
}

std::forward_list<std::unique_ptr<Param>> parse_param_list(TokenStream& ts) {
	auto param = parse_param(ts);

	if (ts.peek_type(1) == Token::Type::Comma) {
		auto param_list = parse_param_list(ts);
		param_list.push_front(std::move(param));
		return param_list;
	} else {
		return std::forward_list{ std::move(param) };
	}
}

std::unique_ptr<Param> parse_param(TokenStream& ts) {
	std::unique_ptr<Param> param;

	param.type = parse_var_type(ts);
	param.name = parse_identifier(ts);

	return std::move(param);
}

std::string parse_identifier(TokenStream& ts) {
	Token& t = ts.next();

	if (t.type == Token::Type::Identifier) {
		return std::string(t.lexeme);
	}

	throw ParseException("Expected identifier, found something else...");
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
		throw ParseException("Expected token of type <actual_type>, instead encountered <expected_type>");
	}
}
