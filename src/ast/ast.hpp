#pragma once

#include <string>

class ASTVisitor;

class ASTNode {
public:
	virtual void accept_visitor(ASTVisitor& visitor) = 0;
	virtual ~ASTNode() {}
};
