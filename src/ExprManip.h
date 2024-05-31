#pragma once
#include "ExprNode.h"
#include "IdentityDB.h"

namespace ExprManip {
	ExprNode ApplyIdentity(const Identity* identity, const IdentityVarMap& ivm);

	ExprNode Simplify(const ExprNode& expr);
}