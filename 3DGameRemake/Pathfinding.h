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

void InitNode(int modelhandle);
std::vector<int> FindPath(int startId, int goalId, std::vector<Node>& mapnode);