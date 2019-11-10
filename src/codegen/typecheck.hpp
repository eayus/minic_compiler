#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include <array>
#include "../ast/declaration.hpp"
#include "../ast/expr.hpp"
#include "../ast/statement.hpp"

using namespace ast::declaration;
using namespace ast::expr;
using namespace ast::statement;

class TypeChecker {
public:
	// Typechecking of expressions
	VarType typeof_expr(Expr& expr);
	VarType typeof_unary_expr(UnaryExpr& expr);
	VarType typeof_binary_expr(BinaryExpr& expr);
	/*VarType typeof_assign_expr(AssignExpr& expr);
	VarType typeof_identifier_expr(Identifier& expr);
	VarType typeof_func_call_expr(FuncCallExpr& expr);
	VarType typeof_int_expr(IntExpr& expr);
	VarType typeof_float_expr(FloatExpr& expr);
	VarType typeof_bool_expr(BoolExpr& expr);*/

private:
	template<std::size_t N>
	void assert_is_one_of(VarType type, std::array<VarType, N>&& valid_types) {
		for (VarType valid_type : valid_types) {
			if (type == valid_type) return;
		}

		std::string error_msg = "Type Mismatch.\nGot: ";
		error_msg += var_type_to_str(type);
		error_msg += "\nExpected:";
		for (VarType valid_type : valid_types) {
			error_msg += var_type_to_str(valid_type);
			error_msg += "\n";
		}
		error_msg += "\n";

		throw std::runtime_error(error_msg);
	}
};
