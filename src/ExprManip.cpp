#include "ExprManip.h"

ExprNode ExprManip::Simplify(const ExprNode& expr) {
	
	ExprNode result = expr;

	for (ExprNode& child : result.children)
		child = Simplify(child);

	if (result.op) {
		int expectedChildren = IS_OP_UNARY(result.op) ? 1 : 2;
		if (result.children.size() == expectedChildren) {
			ExprNode* a = result[0];
			ExprNode* b = result[1];

			if (a->IsLeafVal() && a->val.IsInt() && (!b || (b->IsLeafVal() && b->val.IsInt()))) {
				
				if (result.op == OP_DIV) {

					Int& av = a->val.intVal;
					Int& bv = b->val.intVal;
	
					// Divide by greatest common diviser
					Int gcd = Int::GCD(av, bv);
					if (gcd != 0) {
						av /= gcd;
						bv /= gcd;
					}

				} else {

					bool evaluated = true;
					Int resultNum;

					switch (result.op) {
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

					if (evaluated)
						result = ExprNode(resultNum);
				}
			}

		}
	} else if (result.children.size() == 1) {
		ExprNode firstNode = result.children[0];
		result = firstNode;
	}

	return result;
}