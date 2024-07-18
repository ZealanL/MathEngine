#pragma once
#include "BaseTypes/Value.h"
#include "BaseTypes/Operator.h"

// Expression node
// Used to construct expression trees
class ExprNode {
protected:
	uint64_t hash = 0;

public:
	Value val = {}; // Optional value, if this is a leaf node
	int op = OP_NONE;
	std::vector<ExprNode> children;

	ExprNode() {};

	explicit ExprNode(const Value& val) : val(val), op(OP_NONE) {}
	explicit ExprNode(int op, const std::vector<ExprNode>& children) : op(op), children(children) {}

	uint64_t GetHash() const { return hash; };
	uint64_t UpdateHash();

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

	bool Exists() const {
		return op || !IsLeaf() || val.Defined();
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

	// TODO: Replace with iterator, maybe?
	void ApplyRecursive(std::function<void(const ExprNode*)> func) const {
		func(this);
		for (auto& child : children)
			child.ApplyRecursive(func);
	}

	int CalcTreeSize() const {
		// I *would* use ApplyRecursive but I don't trust the compiler enough to optimize it
		int size = 0;
		for (auto& child : children)
			size += 1 + child.CalcTreeSize();
		return size;
	}

	int CalcTreeDepth() const {
		// I *would* use ApplyRecursive but I don't trust the compiler enough to optimize it
		int size = 1;
		for (auto& child : children)
			size = MAX(size, 1 + child.CalcTreeDepth());
		return size;
	}

	std::string ToString() const;
	PRINTABLE(ExprNode);

	std::string _ChildToString(int index) const;
};