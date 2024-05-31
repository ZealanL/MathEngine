#pragma once
#include "ExprNode.h"

struct Identity {
	ExprNode lhs = {}, rhs = {};
	std::vector<Identity> children;

	// If the rhs introduces a new variable not present in the lhs
	bool isGenerative = false;

	Identity() = default;
	Identity(const ExprNode& lhs, const ExprNode& rhs);

	// If expr fufills the operators of the lhs
	// E.x.: "x+(x*x)" fufills "x+x"
	// E.x.: "x+x" does not fufill "x*x"
	// E.x.: "x+x" does not fufill "x+(x*x)"
	// NOTE: If !Valid(), anything fufills us
	bool Fufills(const ExprNode& expr) const;

	bool Valid() const {
		return lhs.Exists() || rhs.Exists();
	}

	std::string ToString() const {
		return lhs.ToString() + " = " + rhs.ToString();
	}
	PRINTABLE(Identity);
};

struct IdentityTree {
	struct Node {
		Identity identity;
		std::vector<Node> children;

		// Returns true if added
		bool TryAdd(Node& node);

		void _PrintRecursive(std::stringstream& stream, int level) const;
	};

	std::vector<Node> roots = {};

	void AddIdentity(const Identity& identity);

	std::string ToString() const {
		std::stringstream stream;
		for (auto& root : roots)
			root._PrintRecursive(stream, 0);
		return stream.str();
	}
	PRINTABLE(IdentityTree);
};

// Each variable can only have 1 value in an identity
// For instance, in the identity "a+a = 2a", 'a' must be equal to all other 'a's
// So when we are checking an identity, we need to keep track of what each variable is as we go
typedef std::unordered_map<int64_t, const ExprNode*> IdentityVarMap;

// Database holding identities
class IdentityDB {
protected:
	IdentityTree identities;

public:
	void Load(std::filesystem::path path);
	std::vector<std::pair<const Identity*, IdentityVarMap>> FindIdentities(const ExprNode& expr);
};

extern IdentityDB g_IdentityDB;