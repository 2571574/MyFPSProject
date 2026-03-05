#include "Pathfinding.h"
#include <queue>
#include<unordered_map>
#include<algorithm>
#include "Debug.h"
void InitNode(std::vector<Node>&mapnode) {
	Node n0 = { 0,VGet(20,0,25), {1,7,10} };
	Node n1 = { 1,VGet(0,0,25),{0,2,12} };
	Node n2 = { 2,VGet(-20,0,25), {1,3,11,12} };
	Node n3 = { 3,VGet(-20,0,0), {2,4} };
	Node n4 = { 4,VGet(-20,0,-25), {3,5,9} };
	Node n5 = { 5,VGet(0,0,-25), {4,6,8,9,13} };
	Node n6 = { 6,VGet(20,0,-25), {5,7,8,13} };
	Node n7 = { 7,VGet(20,0,0), {0,6} };
	Node n8 = { 8,VGet(22,0,-27), {5,6} };
	Node n9 = { 9,VGet(-22,0,-27), {4,5} };
	Node n10 = { 10,VGet(27,0,17), {0,25} };
	Node n11 = { 11,VGet(-27,0,17), {2,28} };
	Node n12 = { 12,VGet(-12,0,16), {1,2,14} };
	Node n13 = { 13,VGet(12,0,-16), {5,6,15} };
	Node n14 = { 14,VGet(-12,8,-13), {12,20} };
	Node n15 = { 15,VGet(12,8,13), {13,16} };
	Node n16 = { 16,VGet(5,8,13), {15,17,19} };
	Node n17 = { 17,VGet(0,8,13), {16,18,21} };
	Node n18 = { 18,VGet(-5,8,13), {17,20} };
	Node n19 = { 19,VGet(5,8,-13), {16,20} };
	Node n20 = { 20,VGet(-5,8,-13), {14,18,19} };
	Node n21 = { 21,VGet(0,20,-10), {17,22} };
	Node n22 = { 22,VGet(0,20,-27), {21,23,24} };
	Node n23 = { 23,VGet(27,20,-27), {10,22} };
	Node n24 = { 24,VGet(-27,20,-27), {11,22} };
	Node n25 = { 25,VGet(27,5,5), {10,26} };
	Node n26 = { 26,VGet(27,10,-5), {25,27} };
	Node n27 = { 27,VGet(27,15,-15), {26,23} };
	Node n28 = { 28,VGet(-27,5,5), {11,29} };
	Node n29 = { 29,VGet(-27,10,-5), {28,30} };
	Node n30 = { 30,VGet(-27,15,-15), {29,24} };

	mapnode.push_back(n0);
	mapnode.push_back(n1);
	mapnode.push_back(n2);
	mapnode.push_back(n3);
	mapnode.push_back(n4);
	mapnode.push_back(n5);
	mapnode.push_back(n6);
	mapnode.push_back(n7);
	mapnode.push_back(n8);
	mapnode.push_back(n9);
	mapnode.push_back(n10);
	mapnode.push_back(n11);
	mapnode.push_back(n12);
	mapnode.push_back(n13);
	mapnode.push_back(n14);
	mapnode.push_back(n15);
	mapnode.push_back(n16);
	mapnode.push_back(n17);
	mapnode.push_back(n18);
	mapnode.push_back(n19);
	mapnode.push_back(n20);
	mapnode.push_back(n21);
	mapnode.push_back(n22);
	mapnode.push_back(n23);
	mapnode.push_back(n24);
	mapnode.push_back(n25);
	mapnode.push_back(n26);
	mapnode.push_back(n27);
	mapnode.push_back(n28);
	mapnode.push_back(n29);
	mapnode.push_back(n30);
	
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