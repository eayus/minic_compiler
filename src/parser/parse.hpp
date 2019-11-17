#pragma once

#include <memory>
#include <forward_list>
#include "../ast/declaration.hpp"
#include "../ast/expr.hpp"
#include "token_stream.hpp"

using namespace ast::declaration;

class Parser {
public:
	Parser(TokenStream& ts) noexcept;

	std::unique_ptr<Program> parse_program();
	std::forward_list<std::unique_ptr<Declaration>> parse_decl_list();
	std::unique_ptr<Declaration> parse_decl();
	std::unique_ptr<Block> parse_block();
	std::forward_list<std::unique_ptr<VarDecl>> parse_local_decl_list();
	std::unique_ptr<VarDecl> parse_local_decl();
	std::forward_list<std::unique_ptr<Statement>> parse_stmt_list();
	std::unique_ptr<Statement> parse_stmt();
	std::forward_list<std::unique_ptr<ExternDecl>> parse_extern_list();
	std::unique_ptr<ExternDecl> parse_extern();
	std::forward_list<std::unique_ptr<Param>> parse_params();
	std::forward_list<std::unique_ptr<Param>> parse_param_list();
	std::unique_ptr<Param> parse_param();
	std::string parse_identifier(const char* context);
	VarType parse_var_type(const char* context);
	std::unique_ptr<Statement> parse_expr_stmt();
	std::unique_ptr<Statement> parse_if_stmt();
	std::unique_ptr<Statement> parse_while_stmt();
	std::unique_ptr<Statement> parse_return_stmt();
	std::unique_ptr<Expr> parse_expr();
	std::unique_ptr<Expr> parse_bin_op(std::unique_ptr<Expr> lhs, unsigned int min_precedence);
	std::unique_ptr<Expr> parse_primary_expr();
	std::unique_ptr<Expr> parse_not_expr();
	std::unique_ptr<Expr> parse_negate_expr();
	std::unique_ptr<Expr> parse_int_expr();
	std::unique_ptr<Expr> parse_float_expr();
	std::unique_ptr<Expr> parse_bool_expr();
	std::unique_ptr<Expr> parse_parens_expr();
	std::unique_ptr<Expr> parse_assign_expr();
	std::unique_ptr<Expr> parse_func_call_expr();
	std::unique_ptr<Expr> parse_identifier_expr();
	std::forward_list<std::unique_ptr<Expr>> parse_args();
	std::forward_list<std::unique_ptr<Expr>> parse_arg_list();
	ReturnType parse_return_type();
	void consume(Token::Type expected_type, const char* context, const char* expected);

private:
	TokenStream& ts;
};
