#include "IdentityDB.h"
#include "ExprParser.h"

IdentityDB g_IdentityDB = {};

void IdentityDB::Load(std::filesystem::path path) {
	constexpr const char* L_PREFIX = "IdentityDB::Load(): ";

	LOG(L_PREFIX << "Loading identities from " << path << "...");
	size_t amountBefore = identities.size();

	std::ifstream stream = std::ifstream(path);
	stream >> std::noskipws;

	if (!stream.good())
		ERR(L_PREFIX << "Failed to open file " << path);

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

		ExprNode identity = {};
		try {
			identity = ExprParser::Parse(line);
			if (identity.op != OP_EQ)
				ERR("Identity must contain a top-level equal sign");
		} catch (std::exception& e) {
			ERR_CLOSE(L_PREFIX << "Invalid identity \"" << line << "\", failed to parse: " << e.what());
		}

		identities.push_back(identity);
	}

	LOG(L_PREFIX << (identities.size() - amountBefore) << " identities loaded.");
}

// Each variable can only have 1 value in an identity
// For instance, in the identity "a+a = 2a", 'a' must be equal to all other 'a's
// So when we are checking an identity, we need to keep track of what each variable is as we go
typedef std::unordered_map<int64_t, const ExprNode*> IdentityVarMap;

bool IdentityMatchesRecursive(const ExprNode& expr, const ExprNode& identity, IdentityVarMap& ivm) {

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

std::vector<const ExprNode*> IdentityDB::FindIdentities(const ExprNode& expr) {
	std::vector<const ExprNode*> result = {};

	// TODO: Very slow O(n)
	for (auto& identity : identities) {
		IdentityVarMap ivm = {};
		auto& lhs = identity.children[0];
		if (IdentityMatchesRecursive(expr, lhs, ivm))
			result.push_back(&identity);
	}

	return result;
}