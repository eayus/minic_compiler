#pragma once

#include <boost/utility/string_ref.hpp>

namespace lexer {

	// A class to represent a lexical token
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
		};

		Token();
		Token(Type type, boost::string_ref lexeme, unsigned int line_num, unsigned int column_num);
		void print() const;
		std::string to_string() const;
		static const char* type_to_str(Type type);

		Type type;
		boost::string_ref lexeme; // The string of the token
		unsigned int line_num;
		unsigned int column_num;
	};

}
