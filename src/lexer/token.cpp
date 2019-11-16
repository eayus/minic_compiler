#include "token.hpp"

#include <iostream>

namespace lexer {

	Token::Token() : type(Token::Type::EndOfInput) { }

	Token::Token(Token::Type type, boost::string_ref lexeme, unsigned int line_num, unsigned int column_num) :
       		type(type), lexeme(lexeme), line_num(line_num), column_num(column_num) { }

	const char* Token::type_to_str(Type type) {
		switch (type) {
			case Token::Type::Int: return "type keyword \"int\"";
			case Token::Type::Bool: return "type keyword \"bool\"";
			case Token::Type::Float: return "type keyword \"float\"";
			case Token::Type::Void: return "type keyword \"void\"";
			case Token::Type::IntLit: return "integer";
			case Token::Type::FloatLit: return "float";
			case Token::Type::BoolLit: return "boolean";
			case Token::Type::Extern: return "keyword \"extern\"";
			case Token::Type::If: return "keyword \"if\"";
			case Token::Type::Else: return "keyword \"else\"";
			case Token::Type::While: return "keyword \"while\"";
			case Token::Type::Return: return "keyword \"return\"";
			case Token::Type::Identifier: return "identifier";
			case Token::Type::Equals: return "operator \"==\"";
			case Token::Type::Assign: return "operator \"=\"";
			case Token::Type::LBrace: return "operator \"{\"";
			case Token::Type::RBrace: return "operator \"}\"";
			case Token::Type::LParen: return "operator \"(\"";
			case Token::Type::RParen: return "operator \")\"";
			case Token::Type::SemiColon: return "symbol \";\"";
			case Token::Type::Comma: return "symbol \",\"";
			case Token::Type::BitAnd: return "operator \"&\"";
			case Token::Type::LogicAnd: return "operator \"&&\"";
			case Token::Type::BitOr: return "operator \"|\"";
			case Token::Type::LogicOr: return "operator \"||\"";
			case Token::Type::Not: return "operator \"!\"";
			case Token::Type::NotEqual: return "operator \"!=\"";
			case Token::Type::Less: return "operator \"<\"";
			case Token::Type::LessEqual: return "operator \"<=\"";
			case Token::Type::Greater: return "operator \">\"";
			case Token::Type::GreaterEqual: return "operator \">=\"";
			case Token::Type::Divide: return "operator \"\\\"";
			case Token::Type::Minus: return "operator \"-\"";
			case Token::Type::Plus: return "operator \"+\"";
			case Token::Type::Asterisk: return "operator \"*\"";
			case Token::Type::Percent: return "operator \"%\"";
			case Token::Type::EndOfInput: return "end of input";
		}
	}

	std::string Token::to_string() const {
		switch (this->type) {
			case Token::Type::IntLit:
			case Token::Type::FloatLit:
			case Token::Type::BoolLit:
				return std::string(Token::type_to_str(this->type)) + " " + std::string(this->lexeme);
			case Token::Type::Identifier:
				return std::string("identifier \"") + std::string(this->lexeme) + "\"";
			default:
				return std::string(Token::type_to_str(this->type));
		}
	}

	void Token::print() const {
		std::cout << "type: \"" << Token::type_to_str(this->type) << "\", lexeme: \"" << this->lexeme << "\", line: " << this->line_num << ", column: " << this->column_num << std::endl;
	}


}
