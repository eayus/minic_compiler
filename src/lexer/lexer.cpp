#include "lexer.hpp"

#include <cctype>
#include <iostream>
#include <boost/utility/string_ref.hpp>
#include "lexical_error.hpp"

namespace lexer {

	Lexer::Lexer(boost::string_ref str) :
		i(0), str(str), current_line(1), last_newline_index(-1) { }

	void Lexer::lex(std::vector<Token>& tokens) {
		while (true) {
			Token tok = this->next_token();
			tokens.push_back(tok);

			if (tok.type == lexer::Token::Type::EndOfInput) {
				break;
			}
		}
	}

	Token Lexer::next_token() {
		while (std::isspace(str[i])) {
			if (str[i] == '\n') {
				this->current_line += 1;
				this->last_newline_index = i;
			}

			i++;
		}


		// Identifiers and Keywords
		if (std::isalpha(str[i]) || str[i] == '_') {
			size_t token_start = i++;

			while (std::isalnum(str[i]) || str[i] == '_') i++;


			boost::string_ref identifier = str.substr(token_start, i - token_start);

			unsigned int column_num = token_start - this->last_newline_index;

			if (identifier == boost::string_ref("int"))    return Token(Token::Type::Int,     identifier, this->current_line, column_num);
			if (identifier == boost::string_ref("bool"))   return Token(Token::Type::Bool,    identifier, this->current_line, column_num);
			if (identifier == boost::string_ref("float"))  return Token(Token::Type::Float,   identifier, this->current_line, column_num);
			if (identifier == boost::string_ref("void"))   return Token(Token::Type::Void,    identifier, this->current_line, column_num);
			if (identifier == boost::string_ref("extern")) return Token(Token::Type::Extern,  identifier, this->current_line, column_num);
			if (identifier == boost::string_ref("if"))     return Token(Token::Type::If,      identifier, this->current_line, column_num);
			if (identifier == boost::string_ref("else"))   return Token(Token::Type::Else,    identifier, this->current_line, column_num);
			if (identifier == boost::string_ref("while"))  return Token(Token::Type::While,   identifier, this->current_line, column_num);
			if (identifier == boost::string_ref("return")) return Token(Token::Type::Return,  identifier, this->current_line, column_num);
			if (identifier == boost::string_ref("true"))   return Token(Token::Type::BoolLit, identifier, this->current_line, column_num);
			if (identifier == boost::string_ref("false"))  return Token(Token::Type::BoolLit, identifier, this->current_line, column_num);

			return Token(Token::Type::Identifier, identifier, this->current_line, column_num);
		}


		
		
		// Single Symbols
		{
			unsigned int column_num = i - this->last_newline_index;
			switch (str[i]) {
				case '{': return Token(Token::Type::LBrace,    str.substr(i++, 1), this->current_line, column_num);
				case '}': return Token(Token::Type::RBrace,    str.substr(i++, 1), this->current_line, column_num);
				case '(': return Token(Token::Type::LParen,    str.substr(i++, 1), this->current_line, column_num);
				case ')': return Token(Token::Type::RParen,    str.substr(i++, 1), this->current_line, column_num);
				case ';': return Token(Token::Type::SemiColon, str.substr(i++, 1), this->current_line, column_num);
				case ',': return Token(Token::Type::Comma,     str.substr(i++, 1), this->current_line, column_num);
				case '-': return Token(Token::Type::Minus,     str.substr(i++, 1), this->current_line, column_num);
				case '+': return Token(Token::Type::Plus,      str.substr(i++, 1), this->current_line, column_num);
				case '*': return Token(Token::Type::Asterisk,  str.substr(i++, 1), this->current_line, column_num);
				case '%': return Token(Token::Type::Percent,   str.substr(i++, 1), this->current_line, column_num);
				default: break;
			}
		}




		// Equals and Assign
		if (str[i] == '=') {
			if (str[i + 1] == '=') {
				i += 2;
				return Token(Token::Type::Equals, str.substr(i - 2, 2), this->current_line, i - 2 - this->last_newline_index);
			} else {
				i += 1;
				return Token(Token::Type::Assign, str.substr(i - 1, 1), this->current_line, i - 1 - this->last_newline_index);
			}
		}


		
		// Logical and Bitwise And
		if (str[i] == '&') {
			if (str[i + 1] == '&') {
				i += 2;
				return Token(Token::Type::BitAnd, str.substr(i - 2, 2), this->current_line, i - 2 - this->last_newline_index);
			} else {
				i += 1;
				return Token(Token::Type::LogicAnd, str.substr(i - 1, 1), this->current_line, i - 1 - this->last_newline_index);
			}
		}


		// Logical and Bitwise Or
		if (str[i] == '|') {
			if (str[i + 1] == '|'){
				i += 2;
				return Token(Token::Type::BitOr, str.substr(i - 2, 2), this->current_line, i - 2 - this->last_newline_index);
			} else {
				i += 1;
				return Token(Token::Type::LogicOr, str.substr(i - 1, 1), this->current_line, i - 1 - this->last_newline_index);
			}
		}


		// Not and NotEqual
		if (str[i] == '!') {
			if (str[i + 1] == '=') {
				i += 2;
				return Token(Token::Type::NotEqual, str.substr(i - 2, 2), this->current_line, i - 2 - this->last_newline_index);
			} else {
				i += 1;
				return Token(Token::Type::Not, str.substr(i - 1, 1), this->current_line, i - 1 - this->last_newline_index);
			}
		}


		// Less and LessEqual
		if (str[i] == '<') {
			if (str[i + 1] == '=') {
				i += 2;
				return Token(Token::Type::LessEqual, str.substr(i - 2, 2), this->current_line, i - 2 - this->last_newline_index);
			} else {
				i += 1;
				return Token(Token::Type::Less, str.substr(i - 1, 1), this->current_line, i - 1 - this->last_newline_index);
			}
		}


		// Greater and GreaterEqual
		if (str[i] == '>') {
			if (str[i + 1] == '=') {
				i += 2;
				return Token(Token::Type::GreaterEqual, str.substr(i - 2, 2), this->current_line, i - 2 - this->last_newline_index);
			} else {
				i += 1;
				return Token(Token::Type::Greater, str.substr(i - 1, 1), this->current_line, i - 1 - this->last_newline_index);
			}
		}


		// Divison and Comments
		if (str[i] == '/') {
			if (str[i + 1] == '/') {
				i += 2;

				while (str[i] != '\n' && str[i] != '\r' && str[i] != '\0') i++;
				this->last_newline_index = i;
				i++;
				this->current_line += 1;

				return this->next_token();
			} else {
				i += 1;
				return Token(Token::Type::Divide, str.substr(i - 1, 1), this->current_line, i - 1 - this->last_newline_index);
			}
		}


		
		// Floating Point Numbers
		if (str[i] == '.') {
			size_t token_start = i++;

			while (std::isdigit(str[i])) i++;

			return Token(Token::Type::FloatLit, str.substr(token_start, i - token_start), this->current_line, token_start - this->last_newline_index);
		}

		// Mixed Numbers
		if (std::isdigit(str[i])) {
			size_t token_start = i++;

			while (std::isdigit(str[i])) i++;

			if (str[i] == '.') {
				// Floating point
				i++;

				while(std::isdigit(str[i])) i++;
				
				return Token(Token::Type::FloatLit, str.substr(token_start, i - token_start), this->current_line, token_start - this->last_newline_index);

			} else { 
				// Integers
				return Token(Token::Type::IntLit, str.substr(token_start, i - token_start), this->current_line, token_start - this->last_newline_index);
			}
		}


		if (str[i] == '\0') {
			return Token(Token::Type::EndOfInput, "", this->current_line, i - this->last_newline_index);
		}

		// Token is invalid
		size_t token_start = i;
		while (!std::isspace(str[i]) && str[i] != '\0') {
			i += 1;
		}
		auto invalid_str = str.substr(token_start, i - token_start);

		throw LexicalError(invalid_str, this->current_line, token_start - this->last_newline_index);
	}
}
