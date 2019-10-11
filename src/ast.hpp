#pragma once

class Program {
	std::vector<std::variant<ExternDecl, VarDecl, FunctionDecl>> decls;
}
