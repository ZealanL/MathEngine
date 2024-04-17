#include "ExprParser.h"

std::vector<std::string> Tokenize(const std::string& str) {
	std::vector<std::string> result = {};

	enum {
		TYPE_ALPHA,
		TYPE_NUM,
		TYPE_OTHER
	};

	int lastType = TYPE_OTHER;
	for (char c : str) {

		int type;
		if (isalpha(c)) {
			type = TYPE_ALPHA;
		} else if (isdigit(c) || (c == '.')) {
			type = TYPE_NUM;
		} else {
			type = TYPE_OTHER;
		}

		bool continueToken = false;
		if (type == TYPE_ALPHA || type == TYPE_NUM)
			continueToken = (lastType == type);
		
		if (continueToken) {
			result.back() += c;
		} else if (!isblank(c)) {
			result.push_back(std::string(1, c));
		}

		lastType = type;
	}

	return result;
}

// Makes a flat expression have proper operators
// E.g. the proper version of "5-3" is "5+-3"
// E.g. the proper version of "ab" is "a*b"
void MakeFlatExprProper(ExprNode& flatNode) {
	
	for (int i = 0; i < flatNode.children.size(); i++) {
		ExprNode* node = flatNode[i];

		// Convert subtraction to added negation
		if (!node->IsVal() && node->op == OP_NEG) {
			ExprNode* lhs = flatNode[i - 1];
			ExprNode* rhs = flatNode[i + 1];
			if (lhs && rhs && lhs->IsVal() && rhs->IsVal()) {
				// Subtraction, not negation
				ExprNode additionNode = {};
				additionNode.op = OP_ADD;
				flatNode.AddChild(additionNode, i);
				i++;
			}
		}

		// Convert implicit multiplication
		if (node->IsVal()) {
			ExprNode* prev = flatNode[i - 1];
			if (prev && prev->IsVal()) {
				ExprNode multiplicationNode = {};
				multiplicationNode.op = OP_MUL;
				flatNode.AddChild(multiplicationNode, i);
				i++;
			}
		}
	}
}

// Build a flat set of expression nodes into an expression tree
void BuildExprTreeRecursive(ExprNode& flatNode) {

	for (ExprNode& child : flatNode.children)
		if (child.children.size() > 1)
			BuildExprTreeRecursive(child);

	MakeFlatExprProper(flatNode);

	while (true) {
		int highestPrec = 0;
		int higestPrecIndex = -1;
		for (int i = 0; i < flatNode.children.size(); i++) {
			ExprNode* subNode = flatNode[i];
			if (!subNode->IsVal() && subNode->op) {
				int prec = OP_PREC[subNode->op];

				if (IS_OP_UNARY(subNode->op)) {
					// Check for unary bypass
					int dir = UNARY_OP_DIRECTION(subNode->op);

					// Node on the other side
					int oppositeIndex = i - dir;
					ExprNode* oppositeNode = flatNode[oppositeIndex];
					if (oppositeNode && !oppositeNode->IsVal())
						prec = PREC_MAX;
				}

				if (prec > highestPrec) {
					highestPrec = prec;
					higestPrecIndex = i;
				}
			}
		}

		if (higestPrecIndex != -1) {
			int opIndex = higestPrecIndex;
			ExprNode* opNode = flatNode[opIndex];

			if (IS_OP_UNARY(opNode->op)) {
				int operandIndex = opIndex + UNARY_OP_DIRECTION(opNode->op);
				ExprNode* operand = flatNode[operandIndex];
				if (operand && operand->IsVal()) {
					opNode->AddChild(*operand);
					flatNode.RemoveChild(operandIndex);
				} else {
					ERR("Invalid synax for operator \"" << OP_STR[opNode->op] << "\"");
				}
			} else {
				ExprNode* operands[2] = {
					flatNode[opIndex - 1],
					flatNode[opIndex + 1]
				};

				for (ExprNode* operand : operands) {
					if (operand && operand->IsVal()) {
						opNode->AddChild(*operand);
					} else {
						ERR("Invalid synax for operator \"" << OP_STR[opNode->op] << "\"");
					}		
				}
				flatNode.RemoveChild(opIndex - 1);
				flatNode.RemoveChild(opIndex + 1 - 1); // Subtract 1 because we removed a node before it
			}

		} else {
			break; // Nothing left to do
		}
	}

	// Remove redundant child
	if (flatNode.children.size() == 1 && !flatNode.op) {
		ExprNode child = flatNode.children[0];
		flatNode = child;
	}
}

// Convert tokens to a flat set of expression nodes
ExprNode TokensToExprNode(const std::vector<std::string>& tokens) {
	ExprNode baseNode = {};
	std::stack<ExprNode*> curNodes;
	curNodes.push(&baseNode);

	for (auto& token : tokens) {
		auto curNode = curNodes.top();

		char c = token[0];

		if (isdigit(c)) {
			ExprNode numNode = ExprNode(Int::FromString(token));
			curNode->AddChild(numNode);
		} else if (isalpha(c)) {
			ExprNode varNode = ExprNode(Variable(token));
			curNode->AddChild(varNode);
		} else if (token == "(") {
			curNode->AddChild({});
			curNodes.push(&curNode->children.back());
		} else if (token == ")") {
			if (curNodes.size() > 1) {
				curNodes.pop();
			} else {
				throw std::runtime_error("Unmatched closing parenthesis");
			}
		} else {
			// Operator
			int op = OP_NONE;
			for (int i = 1; i < OPERATOR_AMOUNT; i++) {
				if (token == OP_STR[i]) {
					op = i;
					break;
				}
			}

			if (op == OP_NONE)
				throw std::runtime_error("Unknown operator: " + token);

			ExprNode opNode = {};
			opNode.op = op;
			curNode->AddChild(opNode);
		}
	}

	BuildExprTreeRecursive(baseNode);

	return baseNode;
}

ExprNode ExprParser::Parse(const std::string& str) {
	std::vector<std::string> tokens = Tokenize(str);
	return TokensToExprNode(tokens);
}