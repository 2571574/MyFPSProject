#include "Pathfinding.h"
#include <queue>
#include<unordered_map>
#include<algorithm>
#include "Debug.h"
void InitNode(int modelhandle, std::vector<Node>& mapnode) {
	float gridSize = 2.0f;
	float startY = 30.0f;
	float endY = -20.0f;

	float maxSlopeNormal = 0.6f;
	float charaHeight = 2.0f;
	float charaRadius = 0.5f;
	float maxStepHeight = 0.5f;

	float minX = -29.0f;	float maxX = 29.0f;
	float minZ = -29.0f;	float maxZ = 29.0f;

	for (int x = minX; x <= maxX; x += gridSize) {
		for (int z = minZ; z <= maxZ; z += gridSize) {
			VECTOR rayStart = VGet(x, startY, z);
			VECTOR rayEnd = VGet(x, endY, z);
			while (1) {
				MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(modelhandle, -1, rayStart, rayEnd);
				if (hit.HitFlag == 0) {
					break;
				}

				if (hit.Normal.y >= maxSlopeNormal) {
					VECTOR ceilStart = VGet(hit.HitPosition.x, hit.HitPosition.y + 0.1f, hit.HitPosition.z);
					VECTOR ceilEnd = VGet(hit.HitPosition.x, hit.HitPosition.y + charaHeight + maxStepHeight, hit.HitPosition.z);
					MV1_COLL_RESULT_POLY ceilHit = MV1CollCheck_Line(modelhandle, -1, ceilStart, ceilEnd);
					if (ceilHit.HitFlag == 0) {
						Node node;
						node.position = hit.HitPosition;
						mapnode.push_back(node);
					}
				}

				rayStart = VAdd(hit.HitPosition, VGet(0, -0.1f, 0));
				if (rayStart.y < endY) {
					break;
				}
			}
		}
	}

	float connectDist2D = gridSize * 1.5f;

	for (size_t i = 0; i < mapnode.size(); ++i) {
		for (size_t j = i + 1; j < mapnode.size(); ++j) {
			VECTOR posA = mapnode[i].position;
			VECTOR posB = mapnode[j].position;

			VECTOR aXZ = VGet(posA.x, 0.0f, posA.z);
			VECTOR bXZ = VGet(posB.x, 0.0f, posB.z);
			if (VSize(VSub(aXZ, bXZ)) > connectDist2D) continue;
			if (std::abs(posA.y - posB.y) > maxStepHeight)continue;
			VECTOR capStart = VGet(posA.x, posA.y + charaHeight * 0.5f, posA.z);
			VECTOR capEnd = VGet(posB.x, posB.y + charaHeight * 0.5f, posB.z);

			MV1_COLL_RESULT_POLY_DIM capHit = MV1CollCheck_Capsule(modelhandle, -1, capStart, capEnd, charaRadius);
			if (capHit.HitNum != 0) {
				MV1CollResultPolyDimTerminate(capHit);
				continue;
			}
			MV1CollResultPolyDimTerminate(capHit);

			VECTOR midPos = VGet((posA.x + posB.x) / 2.0f, (posA.y + posB.y) / 2.0f, (posA.z, posB.z) / 2.0f);

			VECTOR midRayStart = VGet(midPos.x, midPos.y + maxStepHeight, midPos.z);
			VECTOR midRayEnd = VGet(midPos.x, midPos.y - maxStepHeight * 2.0f, midPos.z);

			MV1_COLL_RESULT_POLY  gapHit = MV1CollCheck_Line(modelhandle, -1, midRayStart, midRayEnd);
			if (gapHit.HitFlag == 0)continue;

			mapnode[i].connectedNode.push_back(j);
			mapnode[j].connectedNode.push_back(i);
		}
	}
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