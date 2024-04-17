#pragma once
#include "Value.h"
#include "Operator.h"

// Expression node
// Used to construct expression trees
struct ExprNode {
	Value val = {}; // Optional value, if this is a leaf node
	int op = OP_NONE;
	std::vector<ExprNode> children;

	ExprNode() {};

	explicit ExprNode(const Value& val) : val(val), op(OP_NONE) {}
	explicit ExprNode(int op, const std::vector<ExprNode>& children) : op(op), children(children) {}

	bool IsLeaf() const { return children.empty(); }

	// Whether this node can be treated as a value
	// True if it is either a number itself, or the result of an operation
	bool IsVal() const { return (op == OP_NONE) || !children.empty(); }

	bool IsLeafVal() const { return IsLeaf() && IsVal(); }

	ExprNode* operator[](size_t index) {
		if (index < children.size()) {
			return &children[index];
		} else {
			return NULL;
		}
	}

	const ExprNode* operator[](size_t index) const {
		if (index < children.size()) {
			return &children[index];
		} else {
			return NULL;
		}
	}

	bool operator ==(const ExprNode& other) const;
	bool operator !=(const ExprNode& other) const {
		return !(*this == other);
	}

	void AddChild(const ExprNode& child, size_t index) {
		children.insert(children.begin() + index, child);
	}

	void AddChild(const ExprNode& child) {
		children.push_back(child);
	}

	void RemoveChild(size_t index) {
		children.erase(children.begin() + index);
	}

	std::string ToString() const;
	PRINTABLE(ExprNode);

	std::string _ChildToString(int index) const;
};