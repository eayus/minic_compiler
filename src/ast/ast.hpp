#pragma once

#include <string>

class ASTVisitor;

class ASTNode {
public:
	virtual void print_tree(std::string indent_str, bool is_last) const = 0;
	virtual void accept_visitor(ASTVisitor& visitor) = 0;
	virtual ~ASTNode() {}
};
