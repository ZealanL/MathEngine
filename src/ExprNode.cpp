#include "ExprNode.h"

uint64_t ExprNode::UpdateHash() {
	HashBuilder hb = HashBuilder(IsVal(), op, children.size());

	if (IsVal()) {
		hb += val.IsVar();
		if (val.IsVar()) {
			hb += val.varVal.GetHash();
		} else {
			hb += val.intVal.val;
		}
	}

	for (int i = 0; i < children.size(); i++) {
		hb.Add(i);
		children[i].UpdateHash();
		hb.Add(children[i].GetHash());
	}

	hash = hb.Get();
	return hash;
}

bool ExprNode::operator==(const ExprNode& other) const {
	if (op != other.op)
		return false;

	if (!op)
		if (val != other.val)
			return false;

	if (children.size() != other.children.size())
		return false;

	for (int i = 0; i < children.size(); i++)
		if (children[i] != other.children[i])
			return false;

	return true;
}

std::string ExprNode::ToString() const {
	std::stringstream result;
	
	if (op) {
		if (children.empty()) {
			result << "(" << OP_STR[op] << ")";
		} else if (children.size() == 1) {
			result << OP_STR[op];
			result << _ChildToString(0);
		} else if (children.size() == 2) {
			result << _ChildToString(0) << ' ';
			result << OP_STR[op];
			result << ' ' << _ChildToString(1);
		} else {
			ERR_CLOSE("Cannot print operation with " << children.size() << " children");
		}
	} else {
		if (children.size() > 0) {
			result << "(";
			for (int i = 0; i < children.size(); i++) {
				result << (i > 0 ? ", " : "") << children[i];
			}
			result << ")";
		} else {
			result << val;
		}
	}

	return result.str();
}

std::string ExprNode::_ChildToString(int index) const {
	auto& child = children[index];

	bool needsParens = false;
	if (child.op && !child.IsLeaf()) {
		if (IS_OP_UNARY(child.op)) {
			needsParens = OP_PREC[child.op] < OP_PREC[this->op];
		} else {
			if (OP_PREC[child.op] != OP_PREC[this->op]) {
				needsParens = true;
			} else {
				// Make sure to add parentheses to "a ? (a ? a)"
				needsParens = index > 0;
			}
		}
	}

	if (needsParens) {
		return '(' + child.ToString() + ')';
	} else {
		return child.ToString();
	}
}