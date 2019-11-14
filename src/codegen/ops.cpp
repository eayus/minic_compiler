#include "ops.hpp"

using Builder = llvm::IRBuilder<>;
using namespace ast::declaration;
using namespace ast::type;
using llvm::Value;


// Common Types
const FuncType T_ALL_INT = FuncType::binary(VarType::Int, VarType::Int, ReturnType::Int);
const FuncType T_ALL_FLOAT = FuncType::binary(VarType::Float, VarType::Float, ReturnType::Float);
const FuncType T_ALL_BOOL = FuncType::binary(VarType::Bool, VarType::Bool, ReturnType::Bool);
const FuncType T_INT_CMP = FuncType::binary(VarType::Int, VarType::Int, ReturnType::Bool);
const FuncType T_FLOAT_CMP = FuncType::binary(VarType::Float, VarType::Float, ReturnType::Bool);


// BinaryOp::Multiply
const OpTable MULTIPLY_OP_TABLE({
	{ T_ALL_INT,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateMul(lhs, rhs); } },
	{ T_ALL_FLOAT, [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateFMul(lhs, rhs); } }
});

// BinaryOp::Divide
const OpTable DIVIDE_OP_TABLE({
	{ T_ALL_INT,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateSDiv(lhs, rhs); } },
	{ T_ALL_FLOAT, [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateFDiv(lhs, rhs); } }
});

// BinaryOp::Modulo
const OpTable MODULO_OP_TABLE({
	{ T_ALL_INT, [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateSRem(lhs, rhs); } }
});

// BinaryOp::Plus
const OpTable PLUS_OP_TABLE({
	{ T_ALL_INT,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateAdd(lhs, rhs); } },
	{ T_ALL_FLOAT, [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateFAdd(lhs, rhs); } }
});

// BinaryOp::Minus
const OpTable MINUS_OP_TABLE({
	{ T_ALL_INT,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateSub(lhs, rhs); } },
	{ T_ALL_FLOAT, [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateFSub(lhs, rhs); } }
});

// BinaryOp::Less
const OpTable LESS_OP_TABLE({
	{ T_INT_CMP,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateICmpSLT(lhs, rhs); } },
	{ T_FLOAT_CMP, [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateFCmpOLT(lhs, rhs); } }
});

// BinaryOp::LessEqual
const OpTable LESS_EQUAL_OP_TABLE({
	{ T_INT_CMP,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateICmpSLE(lhs, rhs); } },
	{ T_FLOAT_CMP, [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateFCmpOLE(lhs, rhs); } }
});

// BinaryOp::Greater
const OpTable GREATER_OP_TABLE({
	{ T_INT_CMP,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateICmpSGT(lhs, rhs); } },
	{ T_FLOAT_CMP, [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateFCmpOGT(lhs, rhs); } }
});

// BinaryOp::GreaterEqual
const OpTable GREATER_EQUAL_OP_TABLE({
	{ T_INT_CMP,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateICmpSGE(lhs, rhs); } },
	{ T_FLOAT_CMP, [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateFCmpOGE(lhs, rhs); } }
});

// BinaryOp::Equals
const OpTable EQUALS_OP_TABLE({
	{ T_INT_CMP,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateICmpEQ(lhs, rhs); } },
	{ T_FLOAT_CMP, [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateFCmpOEQ(lhs, rhs); } },
	{ T_ALL_BOOL,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateICmpEQ(lhs, rhs); } }
});

// BinaryOp::NotEquals
const OpTable NOT_EQUALS_OP_TABLE({
	{ T_INT_CMP,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateICmpNE(lhs, rhs); } },
	{ T_FLOAT_CMP, [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateFCmpONE(lhs, rhs); } },
	{ T_ALL_BOOL,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateICmpNE(lhs, rhs); } }
});

// BinaryOp::And
const OpTable AND_OP_TABLE({
	{ T_ALL_BOOL,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateAnd(lhs, rhs); } }
});

// BinaryOp::Or
const OpTable OR_OP_TABLE({
	{ T_ALL_BOOL,   [](Builder builder, Value* lhs, Value* rhs) { return builder.CreateOr(lhs, rhs); } }
});



// Function Definitions

const OpTable DEFAULT_OP_TABLE({});


OpTable::OpTable(std::initializer_list<std::pair<FuncType, BuilderFunc>> entries) :
	entries(entries) { }


const OpTable& OpTable::for_op(BinaryOp op) {
	switch (op) {
		case BinaryOp::Multiply: return MULTIPLY_OP_TABLE;
		case BinaryOp::Divide: return DIVIDE_OP_TABLE;
		case BinaryOp::Modulo: return MODULO_OP_TABLE;
		case BinaryOp::Plus: return PLUS_OP_TABLE;
		case BinaryOp::Minus: return MINUS_OP_TABLE;
		case BinaryOp::Less: return LESS_OP_TABLE;
		case BinaryOp::LessEqual: return LESS_EQUAL_OP_TABLE;
		case BinaryOp::Greater: return GREATER_OP_TABLE;
		case BinaryOp::GreaterEqual: return GREATER_EQUAL_OP_TABLE;
		case BinaryOp::Equals: return EQUALS_OP_TABLE;
		case BinaryOp::NotEquals: return NOT_EQUALS_OP_TABLE;
		case BinaryOp::And: return AND_OP_TABLE;
		case BinaryOp::Or: return OR_OP_TABLE;
		default: return DEFAULT_OP_TABLE;
	}
}

