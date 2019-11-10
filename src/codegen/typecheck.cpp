#include "typecheck.hpp"
#include <experimental/array>

using std::experimental::make_array;

VarType TypeChecker::typeof_expr(Expr& expr) {
	if (UnaryExpr* unary_expr = dynamic_cast<UnaryExpr*>(&expr)) {
		return this->typeof_unary_expr(*unary_expr);
	}

	if (BinaryExpr* binary_expr = dynamic_cast<BinaryExpr*(&expr)) {
		return this->typeof_binary_expr(*binary_expr);
	}

	//TODO: remove
	return VarType::Int;
}

VarType TypeChecker::typeof_unary_expr(UnaryExpr& expr) {
	VarType operand_type = this->typeof_expr(*expr.operand);

	switch (expr.op) {
		case UnaryOp::Not:
			this->assert_is_one_of(operand_type, make_array(VarType::Bool));
			return VarType::Bool;

		case UnaryOp::Negate:
			this->assert_is_one_of(operand_type, make_array(VarType::Int, VarType::Float));
			return operand_type;
	}
}

VarType TypeChecker::typeof_binary_expr(BinaryExpr& expr) {
	//TODO: remove
	return VarType::Int;
}
