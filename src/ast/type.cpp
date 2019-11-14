#include "type.hpp"

namespace ast {
namespace type {

FuncType::FuncType(std::vector<VarType> param_types, ReturnType ret_type) :
	param_types(param_types),
	ret_type(ret_type) { }

FuncType FuncType::binary(VarType param1, VarType param2, ReturnType ret_type) {
	return FuncType({ param1, param2 }, ret_type);
}

}
}
