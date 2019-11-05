#pragma once

#include <vector>
#include <boost/utility/string_ref.hpp>

namespace lexer {

	class Token {
	public:
		enum class Type {
			Int,
			Bool,
			Float,
			Void,
			IntLit,
			BoolLit,
			FloatLit,
			Extern,
			If,
			Else,
			While,
			Return,
			Identifier,
			Equals,
			Assign,
			LBrace,
			RBrace,
			LParen,
			RParen,
			SemiColon,
			Comma,
			Minus,
			Plus,
			Asterisk,
			Percent,
			BitAnd,
			LogicAnd,
			BitOr,
			LogicOr,
			Not,
			NotEqual,
			Less,
			LessEqual,
			Greater,
			GreaterEqual,
			Divide,
			EndOfInput,
			Invalid,
		};




		Token(Type type, boost::string_ref lexeme);
		void print();

		static const char* type_to_str(Type type);

		Type type;
		boost::string_ref lexeme;
	};


	class Lexer {
	public:
		Lexer(boost::string_ref str);
		void lex(std::vector<Token>& tokens);

	private:
		Token next_token();
	
	private:
		size_t i;
		boost::string_ref str;
		std::vector<Token> tokens;
	};
}
