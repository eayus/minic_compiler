#pragma once

#include <vector>

namespace ast {
namespace type {

	enum class VarType {
		Int = 0,
		Float = 1,
		Bool = 2
	};

	enum class ReturnType {
		Int = 0,
		Float = 1,
		Bool = 2,
		Void = 3
	};

	struct FuncType {
		FuncType(std::vector<VarType> param_types, ReturnType ret_type);
		static FuncType binary(VarType param1, VarType param2, ReturnType ret_type);
		std::vector<VarType> param_types;
		ReturnType ret_type;
	};

}
}
