#include "IdentityDB.h"
#include "ExprParser.h"

IdentityDB g_IdentityDB = {};

/////////////////////////////////////////////////////

Identity::Identity(const ExprNode& lhs, const ExprNode& rhs) 
	: lhs(lhs), rhs(rhs) {

	// Find all var hashes in lhs
	std::unordered_set<uint64_t> varsInLhs;
	lhs.ApplyRecursive(
		[&](const ExprNode* node) {
			if (node->IsVal() && node->val.IsVar())
				varsInLhs.insert(node->val.varVal.GetHash());
		}
	);

	// If any don't exist in lhs, mark generative
	this->isGenerative = false;
	rhs.ApplyRecursive(
		[&](const ExprNode* node) {
			if (node->IsVal() && node->val.IsVar())
				if (varsInLhs.find(node->val.varVal.GetHash()) == varsInLhs.end())
					this->isGenerative = true;
		}
	);
}

bool FufillsRecursive(const ExprNode& a, const ExprNode& b) {
	if (!a.IsLeaf()) {

		if (a.op != b.op)
			return false;

		if (a.children.size() == b.children.size()) {

			for (int i = 0; i < a.children.size(); i++)
				if (!FufillsRecursive(a.children[i], b.children[i]))
					return false;

		} else {
			return false;
		}
	}

	return true;
}


bool Identity::Fufills(const ExprNode& expr) const {

	if (!Valid())
		return true;

	return FufillsRecursive(lhs, expr);
}

//////////////////////////////////////////////////////

void IdentityTree::AddIdentity(const Identity& identity) {

	Node asNode = { identity };

	for (auto& root : roots) {
		if (root.TryAdd(asNode))
			return;
	}

	roots.push_back(asNode);
}

bool IdentityTree::Node::TryAdd(Node& node) {

	if (!identity.Valid()) {
		*this = node;
		return true;
	}

	bool fufills = identity.Fufills(node.identity.lhs);
	bool backwardsFufills = node.identity.Fufills(identity.lhs);

	if (backwardsFufills) {
		if (!fufills) {
			// Order should be swapped
			std::swap(*this, node);
			return true;
		} else {
			// Equal
			return false;
		}
	}

	if (fufills) {
		for (auto& child : children) {
			if (child.TryAdd(node))
				return true;
		}

		children.push_back(node);
		return true;
	} else {
		return false;
	}
}

void IdentityTree::Node::_PrintRecursive(std::stringstream& stream, int level) const {
	std::string indent = std::string(level * 2, ' ');
	stream << indent << "> " << identity << std::endl;
	for (auto& child : children)
		child._PrintRecursive(stream, level + 1);
}


//////////////////////////////////////////////////////

void IdentityDB::Load(std::filesystem::path path) {
	constexpr const char* L_PREFIX = "IdentityDB::Load(): ";

	LOG(L_PREFIX << "Loading identities from " << path << "...");

	std::ifstream stream = std::ifstream(path);
	stream >> std::noskipws;

	if (!stream.good())
		ERR(L_PREFIX << "Failed to open file " << path);

	size_t numLoaded = 0;

	std::string line;
	while (std::getline(stream, line)) {
		size_t commentIdx = line.find('#');
		if (commentIdx != std::string::npos)
			line = line.substr(0, commentIdx);

		bool isBlank = true;
		for (char c : line)
			if (!isblank(c))
				isBlank = false;
		if (isBlank)
			continue;

		ExprNode identityExpr = {};
		try {
			identityExpr = ExprParser::Parse(line);
			if (identityExpr.op != OP_EQ)
				ERR("Identity must contain a top-level equal sign");
		} catch (std::exception& e) {
			ERR_CLOSE(L_PREFIX << "Invalid identity \"" << line << "\", failed to parse: " << e.what());
		}
		// Add identity and swapped identity
		RASSERT(identityExpr.children.size() == 2);

		auto a = Identity(identityExpr.children[0], identityExpr.children[1]);
		auto b = Identity(identityExpr.children[1], identityExpr.children[0]);
		identities.AddIdentity(a);
		identities.AddIdentity(b);
		allIdentities.push_back(a);
		allIdentities.push_back(b);
		numLoaded++;
	}

	LOG(L_PREFIX << numLoaded << " identities loaded.");
}

bool IdentityDB::IdentityMatchesRecursive(const ExprNode& expr, const ExprNode& identity, IdentityVarMap& ivm) {

	if (identity.IsLeafVal()) {
		// Identity is at a leaf
		// It expects a value

		// Identity wants any variable
		if (identity.val.IsVar()) {
			auto& mappedNode = ivm[identity.val.varVal.GetHash()];

			if (mappedNode) {
				if (expr != *mappedNode)
					return false;
			} else {
				mappedNode = &expr;
			}

			return true;
		} else {
			// It expects a specific value
			if (expr.val != identity.val)
				return false;
		}
	} else {

		// Identity wants a specific operation
		if (expr.op != identity.op)
			return false;

		if (expr.children.size() != identity.children.size())
			return false;

		for (int i = 0; i < expr.children.size(); i++)
			if (!IdentityMatchesRecursive(expr.children[i], identity.children[i], ivm))
				return false;
	}

	return true;
}

void CollectPotentialIdentitiesRecursive(const ExprNode& expr, const IdentityTree::Node& identityNode, std::vector<const Identity*>& identities) {
	if (identityNode.identity.Fufills(expr)) {
		identities.push_back(&identityNode.identity);
		for (auto& subNode : identityNode.children)
			CollectPotentialIdentitiesRecursive(expr, subNode, identities);
	}
}

std::vector<std::pair<const Identity*, IdentityVarMap>> IdentityDB::FindIdentities(const ExprNode& expr) {
	std::vector<const Identity*> potentials = {};
	for (auto& root : g_IdentityDB.identities.roots) {
		CollectPotentialIdentitiesRecursive(expr, root, potentials);
	}

	std::vector<std::pair<const Identity*, IdentityVarMap>> result = {};
	for (auto potential : potentials) {
		IdentityVarMap ivm = {};
		if (IdentityMatchesRecursive(expr, potential->lhs, ivm))
			result.push_back({ potential, ivm });
	}
	
	return result;
}