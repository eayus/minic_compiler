#pragma once

#include <memory>
#include "../ast/declaration.hpp"

using namespace ast::declaration;

std::unique_ptr<Program> parse_program();
