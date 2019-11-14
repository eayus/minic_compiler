#include "lexer.hpp"

#include <cctype>
#include <iostream>
#include <boost/utility/string_ref.hpp>

namespace lexer {

	Token::Token(Token::Type type, boost::string_ref lexeme) :
       		type(type), lexeme(lexeme) { }


	const char* Token::type_to_str(Type type) {
		switch (type) {
			case Token::Type::Int: return "Int";
			case Token::Type::Bool: return "Bool";
			case Token::Type::Float: return "Float";
			case Token::Type::Void: return "Void";
			case Token::Type::IntLit: return "IntLit";
			case Token::Type::FloatLit: return "FloatLit";
			case Token::Type::BoolLit: return "BoolLit";
			case Token::Type::Extern: return "Extern";
			case Token::Type::If: return "If";
			case Token::Type::Else: return "Else";
			case Token::Type::While: return "While";
			case Token::Type::Return: return "Return";
			case Token::Type::Identifier: return "Identifier";
			case Token::Type::Equals: return "Equals";
			case Token::Type::Assign: return "Assign";
			case Token::Type::LBrace: return "LBrace";
			case Token::Type::RBrace: return "RBrace";
			case Token::Type::LParen: return "LParen";
			case Token::Type::RParen: return "RParen";
			case Token::Type::SemiColon: return "SemiColon";
			case Token::Type::Comma: return "Comma";
			case Token::Type::BitAnd: return "BitAnd";
			case Token::Type::LogicAnd: return "LogicAnd";
			case Token::Type::BitOr: return "BitOr";
			case Token::Type::LogicOr: return "LogicOr";
			case Token::Type::Not: return "Not";
			case Token::Type::NotEqual: return "NotEqual";
			case Token::Type::Less: return "Less";
			case Token::Type::LessEqual: return "LessEqual";
			case Token::Type::Greater: return "Greater";
			case Token::Type::GreaterEqual: return "GreaterEqual";
			case Token::Type::Divide: return "Divide";
			case Token::Type::Minus: return "Minus";
			case Token::Type::Plus: return "Plus";
			case Token::Type::Asterisk: return "Asterisk";
			case Token::Type::Percent: return "Percent";
			case Token::Type::EndOfInput: return "EndOfInput";
			case Token::Type::Invalid: return "Invalid";
		}
	}

	void Token::print() {
		std::cout << "type: \"" << Token::type_to_str(this->type) << "\", lexeme: \"" << this->lexeme << "\"" << std::endl;
	}





	Lexer::Lexer(boost::string_ref str) :
		i(0), str(str) { }

	void Lexer::lex(std::vector<Token>& tokens) {
		while (true) {
			Token tok = this->next_token();
			tokens.push_back(tok);
			if (tok.type == lexer::Token::Type::EndOfInput || tok.type == lexer::Token::Type::Invalid) {
				break;
			}
		}
	}

	Token Lexer::next_token() {
		while (std::isspace(str[i])) i++;


		// Identifiers and Keywords
		if (std::isalpha(str[i]) || str[i] == '_') {
			size_t token_start = i++;

			while (std::isalnum(str[i]) || str[i] == '_') i++;


			boost::string_ref identifier = str.substr(token_start, i - token_start);

			if (identifier == boost::string_ref("int"))    return Token(Token::Type::Int,     identifier);
			if (identifier == boost::string_ref("bool"))   return Token(Token::Type::Bool,    identifier);
			if (identifier == boost::string_ref("float"))  return Token(Token::Type::Float,   identifier);
			if (identifier == boost::string_ref("void"))   return Token(Token::Type::Void,    identifier);
			if (identifier == boost::string_ref("extern")) return Token(Token::Type::Extern,  identifier);
			if (identifier == boost::string_ref("if"))     return Token(Token::Type::If,      identifier);
			if (identifier == boost::string_ref("else"))   return Token(Token::Type::Else,    identifier);
			if (identifier == boost::string_ref("while"))  return Token(Token::Type::While,   identifier);
			if (identifier == boost::string_ref("return")) return Token(Token::Type::Return,  identifier);
			if (identifier == boost::string_ref("true"))   return Token(Token::Type::BoolLit, identifier);
			if (identifier == boost::string_ref("false"))  return Token(Token::Type::BoolLit, identifier);

			return Token(Token::Type::Identifier, identifier);
		}


		
		
		// Single Symbols
		switch (str[i]) {
			case '{': return Token(Token::Type::LBrace,    str.substr(i++, 1));
			case '}': return Token(Token::Type::RBrace,    str.substr(i++, 1));
			case '(': return Token(Token::Type::LParen,    str.substr(i++, 1));
			case ')': return Token(Token::Type::RParen,    str.substr(i++, 1));
			case ';': return Token(Token::Type::SemiColon, str.substr(i++, 1));
			case ',': return Token(Token::Type::Comma,     str.substr(i++, 1));
			case '-': return Token(Token::Type::Minus,     str.substr(i++, 1));
			case '+': return Token(Token::Type::Plus,      str.substr(i++, 1));
			case '*': return Token(Token::Type::Asterisk,  str.substr(i++, 1));
			case '%': return Token(Token::Type::Percent,   str.substr(i++, 1));
			default: break;
		}




		// Equals and Assign
		if (str[i] == '=') {
			if (str[i + 1] == '=') {
				i += 2;
				return Token(Token::Type::Equals, str.substr(i - 2, 2));
			} else {
				i += 1;
				return Token(Token::Type::Assign, str.substr(i - 1, 1));
			}
		}


		
		// Logical and Bitwise And
		if (str[i] == '&') {
			if (str[i + 1] == '&') {
				i += 2;
				return Token(Token::Type::BitAnd, str.substr(i - 2, 2));
			} else {
				i += 1;
				return Token(Token::Type::LogicAnd, str.substr(i - 1, 1));
			}
		}


		// Logical and Bitwise Or
		if (str[i] == '|') {
			if (str[i + 1] == '|'){
				i += 2;
				return Token(Token::Type::BitOr, str.substr(i - 2, 2));
			} else {
				i += 1;
				return Token(Token::Type::LogicOr, str.substr(i - 1, 1));
			}
		}


		// Not and NotEqual
		if (str[i] == '!') {
			if (str[i + 1] == '=') {
				i += 2;
				return Token(Token::Type::NotEqual, str.substr(i - 2, 2));
			} else {
				i += 1;
				return Token(Token::Type::Not, str.substr(i - 1, 1));
			}
		}


		// Less and LessEqual
		if (str[i] == '<') {
			if (str[i + 1] == '=') {
				i += 2;
				return Token(Token::Type::LessEqual, str.substr(i - 2, 2));
			} else {
				i += 1;
				return Token(Token::Type::Less, str.substr(i - 1, 1));
			}
		}


		// Greater and GreaterEqual
		if (str[i] == '>') {
			if (str[i + 1] == '=') {
				i += 2;
				return Token(Token::Type::GreaterEqual, str.substr(i - 2, 2));
			} else {
				i += 1;
				return Token(Token::Type::Greater, str.substr(i - 1, 1));
			}
		}


		// Divison and Comments
		if (str[i] == '/') {
			if (str[i + 1] == '/') {
				i += 2;

				while (str[i] != '\n' && str[i] != '\r' && str[i] != '\0') i++;
				i++;

				return this->next_token();
			} else {
				i += 1;
				return Token(Token::Type::Divide, str.substr(i - 1, 1));
			}
		}


		
		// Floating Point Numbers
		if (str[i] == '.') {
			size_t token_start = i++;

			while (std::isdigit(str[i])) i++;

			return Token(Token::Type::FloatLit, str.substr(token_start, i - token_start));
		}

		// Mixed Numbers
		if (std::isdigit(str[i])) {
			size_t token_start = i++;

			while (std::isdigit(str[i])) i++;

			if (str[i] == '.') {
				// Floating point
				i++;

				while(std::isdigit(str[i])) i++;
				
				return Token(Token::Type::FloatLit, str.substr(token_start, i - token_start));

			} else { 
				// Integers
				return Token(Token::Type::IntLit, str.substr(token_start, i - token_start));
			}
		}


		if (str[i] == '\0') {
			return Token(Token::Type::EndOfInput, "");
		}

		return Token(Token::Type::Invalid, "");
	}
}
