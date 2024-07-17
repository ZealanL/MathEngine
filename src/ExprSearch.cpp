#include "ExprSearch.h"

struct AStarNode {
	ExprNode expr;
	bool hashUpdated = false;

	AStarNode* parent = NULL;
	std::vector<AStarNode*> neighbors;
	bool neighborsGenerated = false;

	float gCost; // Distance from start
	float hCost; // Distance from end
	float fCost; // gCost + hCost

	bool closed = false;

	AStarNode(const ExprNode& expr, float gCost, float hCost)
		: expr(expr), gCost(gCost), hCost(hCost), fCost(gCost + hCost) {
		this->expr.UpdateHash();
	}

	bool operator<(const AStarNode& other) const {
		return fCost < other.fCost;
	}

	friend std::ostream& operator<<(std::ostream& stream, const AStarNode& node) {
		stream << "[\"" << node.expr << "\" | " << node.gCost << " + " << node.hCost << " = " << node.fCost << "]";
		return stream;
	}
};

ExprSearch::Result ExprSearch::FindBest(const ExprNode& expr, SearchDistFn distFn, int64_t maxItrs) {
	// Based on: https://github.com/SebLague/Pathfinding/blob/master/Episode%2003%20-%20astar/Assets/Scripts/Pathfinding.cs

	auto nodeCmpFn = [](const AStarNode* a, const AStarNode* b) { return *a < *b; };
	std::set<AStarNode*, decltype(nodeCmpFn)> open = {};

	std::unordered_map<int64_t, AStarNode*> nodeLookup = {};

	// Can't use nodeLookup, otherwise a hash collision would cause a memory leak
	std::vector<AStarNode*> allocatedNodes = {};

	// Setup from start
	auto startNode = new AStarNode(expr, 0, distFn(expr));
	allocatedNodes.push_back(startNode);
	nodeLookup[startNode->expr.GetHash()] = startNode;
	open.insert(startNode);
	 
	// Main A* loop
	for (int64_t itrs = 0; itrs < maxItrs; itrs++) {
		if (open.empty())
			break; // Ran out of nodes to search

		// Get next node in open (lowest f-cost)
		AStarNode* cur = *open.begin();
		open.erase(open.begin());
		cur->closed = true;

		// Next cost for neighboring nodes
		float gCostNext = cur->gCost + 1;

		// If needed, generate neighbors
		if (!cur->neighborsGenerated) {
			cur->neighborsGenerated = true;

			auto transforms = ExprManip::FindTransformations(cur->expr);
			for (ExprNode& neighborExpr : transforms) {
				neighborExpr.UpdateHash();

				AStarNode* neighbor;
				auto itr = nodeLookup.find(neighborExpr.GetHash());
				if (itr != nodeLookup.end()) {
					neighbor = itr->second;
				} else {
					// TODO: No need to calculate h-cost of neighbor yet
					// TODO: Repeated hash generation
					neighbor = new AStarNode(neighborExpr, gCostNext, distFn(neighborExpr));
					allocatedNodes.push_back(neighbor);
					nodeLookup[neighborExpr.GetHash()] = neighbor;
				}

				cur->neighbors.push_back(neighbor);
			}
		}
		
		// Add non-closed neighbors to open
		for (auto& neighbor : cur->neighbors) {
			if (neighbor->closed)
				continue;

			// TODO: g-cost comparison will always be false if this neighbour was just generated
			// TODO: Can use assertion attempt to check contains (?)
			if (!open.contains(neighbor) || gCostNext < neighbor->gCost) {
				neighbor->parent = cur;
				open.insert(neighbor);
			}
		}
	}

	// Find best node
	AStarNode* bestNode = startNode;
	float bestH = startNode->hCost;
	for (auto& pair : nodeLookup) {
		auto node = pair.second;
		if (!node->parent)
			continue;

		if (node->hCost < bestH) {
			bestH = node->hCost;
			bestNode = node;
		}
	}

	// Build path
	std::vector<ExprNode> path = {};
	for (AStarNode* cur = bestNode; cur; cur = cur->parent)
		path.push_back(cur->expr);
	std::reverse(path.begin(), path.end());

	int64_t numExplored = allocatedNodes.size();

	// Free nodes
	for (auto node : allocatedNodes)
		delete node;

	return Result{ path, numExplored };
}