#pragma once

#include <memory>
#include <forward_list>
#include "../ast/declaration.hpp"
#include "token_stream.hpp"

using namespace ast::declaration;

std::unique_ptr<Program> parse_program(TokenStream& ts);
std::forward_list<std::unique_ptr<Declaration>> parse_decl_list(TokenStream& ts);
std::unique_ptr<Declaration> parse_decl(TokenStream& ts);
std::unique_ptr<Block> parse_block(TokenStream& ts);
std::forward_list<std::unique_ptr<ExternDecl>> parse_extern_list(TokenStream& ts);
std::unique_ptr<ExternDecl> parse_extern(TokenStream& ts);
std::forward_list<std::unique_ptr<Param>> parse_params(TokenStream& ts);
std::forward_list<std::unique_ptr<Param>> parse_param_list(TokenStream& ts);
std::unique_ptr<Param> parse_param(TokenStream& ts);
std::string parse_identifier(TokenStream& ts);
VarType parse_var_type(TokenStream& ts);
ReturnType parse_return_type(TokenStream& ts);
void consume(TokenStream& ts, Token::Type expected_type);
