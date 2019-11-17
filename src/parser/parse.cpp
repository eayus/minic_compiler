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

	this->consume(Token::Type::EndOfInput, "the list of top level declarations", "the end of input");

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
		decl->name = this->parse_identifier("a function declaration");
		this->consume(Token::Type::LParen, "a function declaration", "a \"(\" to signify the start of the parameter list");
		decl->params = this->parse_params();
		this->consume(Token::Type::RParen, "a function declaration", "a \"(\" to signify the end of the parameter list");
		decl->body = this->parse_block();

		return std::move(decl);
	}

	// parse a non-void function
	auto var_type = this->parse_var_type("a function declaration");
	auto name = this->parse_identifier("a function declaration");

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
				this->consume(Token::Type::RParen, "a function declaration", "a \"(\" to signify the end of the parameter list");
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
	this->consume(Token::Type::LBrace, "a block of code", "a \"{\" to signfiy the start of the code block");

	auto block = llvm::make_unique<Block>();
	block->var_decls = this->parse_local_decl_list();
	block->statements = this->parse_stmt_list();

	this->consume(Token::Type::RBrace, "a block of code", "a \"}\" to end the code block");

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
	local_decl->type = this->parse_var_type("a local variable declaration");
	local_decl->name = this->parse_identifier("a local variable declaration");
	this->consume(Token::Type::SemiColon, "a local variable declaration", "a \";\" to end the local variable declaration");
	return local_decl;
}

std::forward_list<std::unique_ptr<Statement>> Parser::parse_stmt_list() {
	switch (ts.peek_type(1)) {
		case Token::Type::RBrace: {
			std::forward_list<std::unique_ptr<Statement>> stmt_list;
			return stmt_list;
		}

		case Token::Type::While:
		case Token::Type::If:
		case Token::Type::Return: 
		case Token::Type::LBrace:
		case Token::Type::Minus:
		case Token::Type::Not:
		case Token::Type::SemiColon:
		case Token::Type::LParen:
		case Token::Type::Identifier:
		case Token::Type::IntLit:
		case Token::Type::FloatLit:
		case Token::Type::BoolLit: {
			auto stmt = this->parse_stmt();
			auto stmt_list = this->parse_stmt_list();
			stmt_list.push_front(std::move(stmt));
			return stmt_list;
		}

		default:
			throw ParseError(
				this->ts.current_line(),
				this->ts.current_column(),
				"a basic block",
				"a closing \"}\" or beginning of a statement",
				std::vector<Token::Type> {
					Token::Type::RBrace,
					Token::Type::While,
					Token::Type::If,
					Token::Type::Return,
					Token::Type::LBrace,
					Token::Type::Minus,
					Token::Type::Not,
					Token::Type::LParen,
					Token::Type::Identifier,
					Token::Type::IntLit,
					Token::Type::FloatLit,
					Token::Type::BoolLit
				},
				ts.next()
			);
	}
}

std::unique_ptr<Statement> Parser::parse_stmt() {
	switch (ts.peek_type(1)) {
		case Token::Type::While: return this->parse_while_stmt();
		case Token::Type::If: return this->parse_if_stmt();
		case Token::Type::Return: return this->parse_return_stmt();
		case Token::Type::LBrace: return this->parse_block();
		case Token::Type::Minus:
		case Token::Type::Not:
		case Token::Type::LParen:
		case Token::Type::Identifier:
		case Token::Type::IntLit:
		case Token::Type::FloatLit:
		case Token::Type::BoolLit:
		case Token::Type::SemiColon:
			return this->parse_expr_stmt();
		default:
			throw ParseError(
				this->ts.current_line(),
				this->ts.current_column(),
				"a statement",
				"the beginning of a while, if, return, block or expression statement",
				std::vector<Token::Type> {
					Token::Type::RBrace,
					Token::Type::While,
					Token::Type::If,
					Token::Type::Return,
					Token::Type::LBrace,
					Token::Type::Minus,
					Token::Type::Not,
					Token::Type::LParen,
					Token::Type::Identifier,
					Token::Type::IntLit,
					Token::Type::FloatLit,
					Token::Type::BoolLit
				},
				ts.next()
			);
	}
}

std::unique_ptr<Statement> Parser::parse_expr_stmt() {
	auto stmt = llvm::make_unique<ExprStmt>();

	if (ts.peek_type(1) == Token::Type::SemiColon) {
		this->consume(Token::Type::SemiColon, "", "");
		stmt->expr = nullptr;
	} else {
		stmt->expr = this->parse_expr();
		this->consume(Token::Type::SemiColon, "a statement definition", "a \";\" to end the statement");
	}

	return stmt;
}

std::unique_ptr<Statement> Parser::parse_if_stmt() {
	auto stmt = llvm::make_unique<IfElse>();

	this->consume(Token::Type::If, "an if statement", "");
	this->consume(Token::Type::LParen, "an if statement", "a \"(\" to mark the beginning of the conditional expression");
	stmt->cond = this->parse_expr();
	this->consume(Token::Type::RParen, "an if statement", "a \"(\" to mark the end of the conditional expression");
	stmt->if_true = this->parse_block();

	if (ts.peek_type(1) == Token::Type::Else) {
		this->consume(Token::Type::Else, "", "");
		stmt->if_false = this->parse_block();
	}

	return stmt;
}

std::unique_ptr<Statement> Parser::parse_while_stmt() {
	auto stmt = llvm::make_unique<While>();

	this->consume(Token::Type::While, "a while statement", "");
	this->consume(Token::Type::LParen, "a while statement", "a \"(\" to mark the beginning of the conditional expression");
	stmt->cond = this->parse_expr();
	this->consume(Token::Type::RParen, "a while statement", "a \"(\" to mark the end of the conditional expression");
	stmt->body = this->parse_stmt();

	return stmt;
}

std::unique_ptr<Statement> Parser::parse_return_stmt() {
	auto stmt = llvm::make_unique<Return>();
	stmt->line_num = this->ts.current_line();
	stmt->column_num = this->ts.current_column();

	this->consume(Token::Type::Return, "", "");

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
			throw ParseError(
				this->ts.current_line(),
				this->ts.current_column(),
				"the list of top level declarations",
				"an extern declaration, variable delaration or function declaration",
				std::vector<Token::Type> {
					Token::Type::Extern,
					Token::Type::Void,
					Token::Type::Int,
					Token::Type::Float,
					Token::Type::Bool
				},
				ts.next()
			);
	}
}

std::unique_ptr<ExternDecl> Parser::parse_extern() {
	// extern ::= "extern" return_type IDENTIFIER "(" params ")" ";"
	ts.next();

	auto extern_decl = llvm::make_unique<ExternDecl>();

	extern_decl->return_type = this->parse_return_type();
	extern_decl->name = this->parse_identifier("extern declaration");
	this->consume(Token::Type::LParen, "an extern declaration", "a \"(\" to signify the beginning of the parameter list");
	extern_decl->params = this->parse_params();
	this->consume(Token::Type::RParen, "an extern declaration", "a \")\" to signify the end of the parameter list");
	this->consume(Token::Type::SemiColon, "an extern declaration", "a \";\" to end the extern declaration");

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
			throw ParseError(
				this->ts.current_line(),
				this->ts.current_column(),
				"function parameter definition",
				"the first parameter, an empty list or \"void\" (for functions taking no parameters)",
				std::vector<Token::Type> {
					Token::Type::RParen,
					Token::Type::Void,
					Token::Type::Int,
					Token::Type::Float,
					Token::Type::Bool
				},
				ts.next()
			);
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

	param->type = this->parse_var_type("parameter definition");
	param->name = this->parse_identifier("parameter definition");

	return param;
}

std::string Parser::parse_identifier(const char* context) {
	const Token& t = this->ts.next();

	if (t.type == Token::Type::Identifier) {
		return std::string(t.lexeme);
	}

	throw ParseError(
		this->ts.current_column(),
		this->ts.current_line(),
		std::string(context),
		"an identifier",
		std::vector<Token::Type> { Token::Type::Identifier },
		t
	);
}

VarType Parser::parse_var_type(const char* context) {
	const Token& tok = ts.next();
	switch (tok.type) {
		// var_type ::= "int"
		case Token::Type::Int: return VarType::Int;

		// var_type ::= "float"
		case Token::Type::Float: return VarType::Float;

		// var_type ::= "bool"
		case Token::Type::Bool: return VarType::Bool;

		default:
			throw ParseError(
				this->ts.current_line(),
				this->ts.current_column(),
				std::string(context),
				"a type of variable",
				std::vector<Token::Type> {
					Token::Type::Int,
					Token::Type::Float,
					Token::Type::Bool
				},
				tok
			);
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

				lhs = llvm::make_unique<BinaryExpr>(*op, std::move(lhs), std::move(rhs), this->ts.current_line(), this->ts.current_column());

			} else {
				lhs = llvm::make_unique<BinaryExpr>(*op, std::move(lhs), std::move(rhs), this->ts.current_line(), this->ts.current_column());
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
		default: throw ParseError(
			this->ts.current_line(),
			this->ts.current_column(),
			"an expression",
			"unary operator, literal, identifier or sub expression",
			std::vector<Token::Type> {
				Token::Type::Not,
				Token::Type::Minus,
				Token::Type::IntLit,
				Token::Type::FloatLit,
				Token::Type::BoolLit,
				Token::Type::LParen,
				Token::Type::Identifier
			},
			ts.next()
		);
	}
}

std::unique_ptr<Expr> Parser::parse_not_expr() {
	auto line_num = this->ts.current_line();
	auto column_num = this->ts.current_column();

	this->consume(Token::Type::Not, "", "");
	//auto expr = parse_expr(ts);
	auto expr = this->parse_primary_expr();
	return llvm::make_unique<UnaryExpr>(UnaryOp::Not, std::move(expr), line_num, column_num);
}

std::unique_ptr<Expr> Parser::parse_negate_expr() {
	auto line_num = this->ts.current_line();
	auto column_num = this->ts.current_column();

	this->consume(Token::Type::Minus, "", "");
	//auto expr = parse_expr(ts);
	auto expr = this->parse_primary_expr();
	return llvm::make_unique<UnaryExpr>(UnaryOp::Negate, std::move(expr), line_num, column_num);
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
	this->consume(Token::Type::LParen, "", "");
	auto expr = this->parse_expr();
	this->consume(Token::Type::RParen, "a sub expression", "a \")\" to close the opening \"(\"");
	return expr;
}

std::unique_ptr<Expr> Parser::parse_assign_expr() {
	auto assign_expr = llvm::make_unique<AssignExpr>();

	assign_expr->line_num = this->ts.current_line();
	assign_expr->column_num = this->ts.current_column();
	assign_expr->name = this->parse_identifier("an assignment");
	this->consume(Token::Type::Assign, "an assignment", "an \"=\"");
	assign_expr->expr = this->parse_expr();

	return assign_expr;
}

std::unique_ptr<Expr> Parser::parse_func_call_expr() {
	auto fc_expr = llvm::make_unique<FuncCallExpr>();
	
	fc_expr->line_num = this->ts.current_line();
	fc_expr->column_num = this->ts.current_column();

	fc_expr->func_name = this->parse_identifier("a function call");
	this->consume(Token::Type::LParen, "a function call", "a \"(\" to begin the parameter list");
	fc_expr->params = this->parse_args();
	this->consume(Token::Type::RParen, "a function call", "a \")\" to end the parameter list");

	return fc_expr;
}

std::unique_ptr<Expr> Parser::parse_identifier_expr() {
	auto line_num = this->ts.current_line();
	auto column_num = this->ts.current_column();
	return llvm::make_unique<IdentifierExpr>(std::string(this->ts.next().lexeme), line_num, column_num);
}

std::forward_list<std::unique_ptr<Expr>> Parser::parse_args() {
	switch (this->ts.peek_type(1)) {
		case Token::Type::RParen: {
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

void Parser::consume(Token::Type expected_type, const char* context, const char* expected) {
	const Token& tok = this->ts.next();
	Token::Type actual_type = tok.type;
	if (actual_type != expected_type) {
		throw ParseError(
			this->ts.current_line(),
			this->ts.current_column(),
			context,
			expected,
			tok
		);
	}
}
