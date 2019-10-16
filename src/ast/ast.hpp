#pragma once

#include <string>

class ASTNode {
public:
	virtual void print_tree(std::string indent_str, bool is_last) const = 0;
	virtual ~ASTNode() {}
};
