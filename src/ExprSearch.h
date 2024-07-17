#pragma once
#include "ExprManip.h"

namespace ExprSearch {
	typedef std::function<float(const ExprNode& expr)> SearchDistFn;

	struct Result {
		std::vector<ExprNode> path;
		int64_t numExplored;
	};

	ExprSearch::Result FindBest(const ExprNode& expr, SearchDistFn distFn, int64_t maxItrs);
}