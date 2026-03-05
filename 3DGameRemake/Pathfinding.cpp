#include "Pathfinding.h"
#include <queue>
#include<unordered_map>
#include<algorithm>
#include "Debug.h"
void InitNode(int modelhandle) {
	float gridSize = 2.0f;
	float startY = 30.0f;
	float endY = -20.0f;
}

std::vector<int> FindPath(int startID, int goalID, std::vector<Node>&mapNode) {
	std::vector<int> path;	//最終経路

	std::priority_queue < NodeRecord, std::vector<NodeRecord>, std::greater<NodeRecord>> openList;

	std::unordered_map<int, NodeRecord> closedList;

	float initialH = VSize(VSub(mapNode[goalID].position, mapNode[startID].position));
	openList.push({ startID,-1,0.0f,initialH });

	while (!openList.empty()) {
		NodeRecord current = openList.top();
		openList.pop();

		if(current.ID == goalID) {
			closedList[current.ID] = current;
			break;
		}

		if (closedList.find(current.ID) != closedList.end() && closedList[current.ID].costDist <= current.costDist) {
			continue;
		}

		closedList[current.ID] = current;

		for (int neighborID : mapNode[current.ID].connectedNode) {
			float distance = VSize(VSub(mapNode[neighborID].position, mapNode[current.ID].position));

			float newCostD = current.costDist + distance;

			float costH = VSize(VSub(mapNode[goalID].position, mapNode[neighborID].position));
			float newCostT = newCostD + costH;

			openList.push({ neighborID,current.ID,newCostD,newCostT });
		}
	}

	if (closedList.find(goalID) != closedList.end()) {
		int currentID = goalID;
		while (currentID != -1) {
			path.push_back(currentID);
			currentID = closedList[currentID].parentID;
		}

		std::reverse(path.begin(), path.end());
	}
	return path;
}