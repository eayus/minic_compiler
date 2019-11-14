#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <functional>
#include <vector>
#include <utility>
#include <initializer_list>
#include "../ast/type.hpp"
#include "../ast/declaration.hpp"

using namespace ast::type;
using namespace ast::declaration;

using BuilderFunc = std::function<llvm::Value*(llvm::IRBuilder<>&, llvm::Value*, llvm::Value*)>;

struct OpTable {
	OpTable(std::initializer_list<std::pair<FuncType, BuilderFunc>> entries);
	static const OpTable& for_op(BinaryOp op);

	std::vector<std::pair<FuncType, BuilderFunc>> entries;
};
