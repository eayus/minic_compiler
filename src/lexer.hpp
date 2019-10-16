#pragma once

#include <string_view>
#include <vector>

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




		Token(Type type, std::string_view lexeme);
		void print();

		static const char* type_to_str(Type type);

		Type type;
		std::string_view lexeme;
	};


	class Lexer {
	public:
		Lexer(std::string_view str);
		void lex(std::vector<Token>& tokens);

	private:
		Token next_token();
	
	private:
		size_t i;
		std::string_view str;
		std::vector<Token> tokens;
	};
}
