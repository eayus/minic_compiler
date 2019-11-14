#include "type_coerce.hpp"

boost::optional<ConversionFunc> coerce_type(VarType from, VarType to) {
	if (from == to) {
		ConversionFunc res = [](Context ctx, Builder builder, Value val) { return val; };
		return res;
	}

	if (from == VarType::Int && to == VarType::Float) {
		ConversionFunc res = [](Context ctx, Builder builder, Value val) {
			return builder.CreateSIToFP(val, llvm::Type::getFloatTy(ctx));
		};
		return res;
	}

	return boost::none;
}


boost::optional<std::vector<ConversionFunc>> coerce_list(std::vector<VarType> from_types, std::vector<VarType> to_types) {
	std::vector<ConversionFunc> result;

	for (size_t i = 0; i < from_types.size(); i++) {
		VarType from = from_types[i];
		VarType to = to_types[i];

		auto maybe_func = coerce_type(from, to);

		if (maybe_func) {
			result.push_back(*maybe_func);
		} else {
			return boost::none;
		}
	}

	return result;
}
