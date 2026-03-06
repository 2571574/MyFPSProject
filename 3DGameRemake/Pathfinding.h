#pragma once
#include <vector>
#include "DxLib.h"

struct Node {
	VECTOR position;
	std::vector <int> connectedNode;
};

struct NodeRecord {
	bool isClosed = false;
	int parentID = -1;
	float costF = 99999.0f;
	float costG = 99999.0f;

	bool operator>(const NodeRecord& other) const {
		return costF > other.costF;
	}
};

void InitNode(int modelhandle, std::vector<Node>& mapnode);
VECTOR GetNodePosition(int nodeID, std::vector<Node>& mapnode);
float GetDistance(VECTOR a, VECTOR b);
int GetNearestNodeIndex(VECTOR pos, std::vector<Node>& mapnode);
std::vector<VECTOR> FindPath(VECTOR startPos, VECTOR goalPos, std::vector<Node>& mapnode);