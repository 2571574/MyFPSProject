#pragma once
#include <vector>
#include "DxLib.h"

struct Node {
	int ID;
	VECTOR position;
	std::vector <int> connectedNode;
};

struct NodeRecord {
	int ID;
	int parentID;
	float costDist;
	float costTotal;

	bool operator>(const NodeRecord& other) const {
		return costTotal > other.costTotal;
	}
};

std::vector<int> FindPath(int startId, int goalId, const std::vector<Node>& mapnode);
void InitNode(std::vector<Node>&mapnode);