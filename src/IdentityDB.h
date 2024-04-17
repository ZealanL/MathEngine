#pragma once
#include "ExprNode.h"

// Database holding identities
class IdentityDB {
protected:
	std::vector<ExprNode> identities;

public:
	void Load(std::filesystem::path path);
	std::vector<const ExprNode*> FindIdentities(const ExprNode& expr);
};

extern IdentityDB g_IdentityDB;