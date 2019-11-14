#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <functional>
#include <vector>
#include <boost/optional.hpp>
#include "../ast/declaration.hpp"

using Builder = llvm::IRBuilder<>&;
using Context = llvm::LLVMContext&;
using Value = llvm::Value*;
// A function to convert one type to another
using ConversionFunc = std::function<Value(Context, Builder, Value)>;

boost::optional<std::vector<ConversionFunc>> coerce_list(std::vector<VarType>, std::vector<VarType>);
