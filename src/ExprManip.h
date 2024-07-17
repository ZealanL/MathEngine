#pragma once
#include "ExprNode.h"
#include "IdentityDB.h"

namespace ExprManip {
	ExprNode ApplyIdentity(const Identity* identity, const IdentityVarMap& ivm);

	bool SimplifyNumbers(ExprNode& expr);
	std::vector<ExprNode> FindTransformations(const ExprNode& expr);
}