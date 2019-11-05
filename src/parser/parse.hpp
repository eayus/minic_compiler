#pragma once

#include <memory>
#include <forward_list>
#include "../ast/declaration.hpp"
#include "../ast/expr.hpp"
#include "token_stream.hpp"

using namespace ast::declaration;

std::unique_ptr<Program> parse_program(TokenStream& ts);
std::forward_list<std::unique_ptr<Declaration>> parse_decl_list(TokenStream& ts);
std::unique_ptr<Declaration> parse_decl(TokenStream& ts);
std::unique_ptr<Block> parse_block(TokenStream& ts);
std::forward_list<std::unique_ptr<VarDecl>> parse_local_decl_list(TokenStream& ts);
std::unique_ptr<VarDecl> parse_local_decl(TokenStream& ts);
std::forward_list<std::unique_ptr<Statement>> parse_stmt_list(TokenStream& ts);
std::unique_ptr<Statement> parse_stmt(TokenStream& ts);
std::forward_list<std::unique_ptr<ExternDecl>> parse_extern_list(TokenStream& ts);
std::unique_ptr<ExternDecl> parse_extern(TokenStream& ts);
std::forward_list<std::unique_ptr<Param>> parse_params(TokenStream& ts);
std::forward_list<std::unique_ptr<Param>> parse_param_list(TokenStream& ts);
std::unique_ptr<Param> parse_param(TokenStream& ts);
std::string parse_identifier(TokenStream& ts);
VarType parse_var_type(TokenStream& ts);
std::unique_ptr<Statement> parse_stmt(TokenStream& ts);
std::unique_ptr<Statement> parse_expr_stmt(TokenStream& ts);
std::unique_ptr<Statement> parse_if_stmt(TokenStream& ts);
std::unique_ptr<Statement> parse_while_stmt(TokenStream& ts);
std::unique_ptr<Statement> parse_return_stmt(TokenStream& ts);
std::unique_ptr<Expr> parse_expr(TokenStream& ts);
std::unique_ptr<Expr> parse_bin_op(TokenStream& ts, std::unique_ptr<Expr> lhs, unsigned int min_precedence);
std::unique_ptr<Expr> parse_primary_expr(TokenStream& ts);
std::unique_ptr<Expr> parse_not_expr(TokenStream& ts);
std::unique_ptr<Expr> parse_negate_expr(TokenStream& ts);
std::unique_ptr<Expr> parse_int_expr(TokenStream& ts);
std::unique_ptr<Expr> parse_float_expr(TokenStream& ts);
std::unique_ptr<Expr> parse_bool_expr(TokenStream& ts);
std::unique_ptr<Expr> parse_parens_expr(TokenStream& ts);
std::unique_ptr<Expr> parse_assign_expr(TokenStream& ts);
std::unique_ptr<Expr> parse_func_call_expr(TokenStream& ts);
std::unique_ptr<Expr> parse_identifier_expr(TokenStream& ts);
std::forward_list<std::unique_ptr<Expr>> parse_args(TokenStream& ts);
std::forward_list<std::unique_ptr<Expr>> parse_arg_list(TokenStream& ts);
ReturnType parse_return_type(TokenStream& ts);
void consume(TokenStream& ts, Token::Type expected_type);
