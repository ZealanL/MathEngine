#include "ExprManip.h"

void ApplyIdentityRecursive(const Identity* identity, ExprNode& appliedIdentity, const IdentityVarMap& ivm) {

	if (appliedIdentity.IsLeafVal()) {
		// Identity is at a leaf
		// It expects a value

		// Identity wants any variable
		if (appliedIdentity.val.IsVar()) {
			auto varHash = appliedIdentity.val.varVal.GetHash();

#ifdef ME_DEBUG
			if (ivm.find(varHash) == ivm.end()) {
				ERR_CLOSE("ApplyIdentityRecursive(): Failed to find \"" << appliedIdentity.val.varVal << "\" for identity \"" << *identity << "\"");
			}
#endif

			auto& mappedNode = ivm.at(varHash);

			appliedIdentity = *mappedNode;
		}
	} else {
		for (auto& child : appliedIdentity.children)
			ApplyIdentityRecursive(identity, child, ivm);
	}
}

ExprNode ExprManip::ApplyIdentity(const Identity* identity, const IdentityVarMap& ivm) {
	ExprNode appliedIdentity = identity->rhs;
	ApplyIdentityRecursive(identity, appliedIdentity, ivm);
	return appliedIdentity;
}

bool ExprManip::SimplifyNumbers(ExprNode& expr) {
	
	for (ExprNode& child : expr.children)
		if (SimplifyNumbers(child))
			return true;

	if (expr.op) {
		int expectedChildren = IS_OP_UNARY(expr.op) ? 1 : 2;
		if (expr.children.size() == expectedChildren) {
			ExprNode* a = expr[0];
			ExprNode* b = expr[1];

			if (a->IsLeafVal() && a->val.IsInt() && (!b || (b->IsLeafVal() && b->val.IsInt()))) {
				
				if (expr.op == OP_DIV) {

					Int& av = a->val.intVal;
					Int& bv = b->val.intVal;
	
					if (bv != 1) {
						// Divide by greatest common diviser
						Int gcd = Int::GCD(av, bv);
						if (gcd != 0) {
							av /= gcd;
							bv /= gcd;
							return true;
						}
					} else {
						// Denominator is 1
						// TODO: Make this only an identity?
						expr = ExprNode(a->val);
						return true;
					}

				} else {

					bool evaluated = true;
					Int resultNum;

					switch (expr.op) {
					case OP_NEG:
						resultNum = -a->val.intVal;
						break;
					case OP_ADD:
						resultNum = a->val.intVal + b->val.intVal;
						break;
					case OP_MUL:
						resultNum = a->val.intVal * b->val.intVal;
						break;
					default:
						evaluated = false;
					}

					if (evaluated) {
						expr = ExprNode(resultNum);
						return true;
					}
				}
			}

		}
	} else if (expr.children.size() == 1) {
		ExprNode firstNode = expr.children[0];
		expr = firstNode;
		return true;
	}

	return false;
}