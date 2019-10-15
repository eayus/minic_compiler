#pragma once

#include "statement.hpp"

namespace ast::declaration {
	
	using namespace ast::statement;

	enum class VarType {
		Int,
		Float,
		Bool
	};

	enum class ReturnType {
		Void,
		Int,
		Float,
		Bool
	};

	class Declaration {
	};

	struct ExternDecl : public Declaration {
		ReturnType return_type;
		std::vector<VarType> params;
	};

	struct VarDecl : public Declaration {
		VarType type;
		std::string name;
	};

	struct FuncDef : public Declaration {
		ReturnType return_type;
		std::string name;
		std::vector<std::unique_ptr<VarDecl>> params;
		std::unique_ptr<Block> body;
	};

	struct Program {
		std::vector<std::unique_ptr<Declaration>> decls;
	};

}
