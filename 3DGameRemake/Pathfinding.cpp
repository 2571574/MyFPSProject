#include "Pathfinding.h"
#include <queue>
#include<unordered_map>
#include<algorithm>
#include "Debug.h"
void InitNode(int modelhandle, std::vector<Node>& mapnode) {
	float gridSize = 3.0f;
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

			VECTOR midPos = VGet((posA.x + posB.x) / 2.0f, (posA.y + posB.y) / 2.0f, (posA.z + posB.z) / 2.0f);

			VECTOR midRayStart = VGet(midPos.x, midPos.y + maxStepHeight, midPos.z);
			VECTOR midRayEnd = VGet(midPos.x, midPos.y - maxStepHeight * 2.0f, midPos.z);

			MV1_COLL_RESULT_POLY  gapHit = MV1CollCheck_Line(modelhandle, -1, midRayStart, midRayEnd);
			if (gapHit.HitFlag == 0)continue;

			mapnode[i].connectedNode.push_back(j);
			mapnode[j].connectedNode.push_back(i);
		}
	}
}
int GetNearestNodeIndex(VECTOR pos, std::vector<Node>& mapnode) {
	int nearestID = -1;
	float minDistSq = 99999.0f;

	for (int i = 0; i < mapnode.size(); i++) {
		float distSq = GetDistance(pos, mapnode[i].position);
		if (distSq < minDistSq) {
			nearestID = i;
			minDistSq = distSq;
		}
	}

	return nearestID;
}

VECTOR GetNodePosition(int nodeID, std::vector<Node>& mapnode) {
	if (nodeID >= 0 && nodeID < mapnode.size()) {
		return mapnode[nodeID].position;
	}
	return VGet(0.0f, 0.0f, 0.0f);
}

float GetDistance(VECTOR a, VECTOR b) { return VSize(VSub(a, b)); };

std::vector<VECTOR>FindPath(VECTOR startPos, VECTOR goalPos, std::vector<Node>&mapnode) {
	std::vector<VECTOR> path;
	if (mapnode.empty()) return path;
	int startIndex = GetNearestNodeIndex(startPos, mapnode);
	int goalIndex = GetNearestNodeIndex(goalPos, mapnode);

	if (startIndex == -1 || goalIndex == -1) {
		return path;
	}

	std::vector<NodeRecord> nodeRecords(mapnode.size());
	using P = std::pair<float, int>;
	std::priority_queue <P, std::vector<P>, std::greater<P>> openList;

	nodeRecords[startIndex].costG = 0.0f;
	nodeRecords[startIndex].costF = GetDistance(mapnode[startIndex].position, mapnode[goalIndex].position);
	openList.push({ nodeRecords[startIndex].costF,startIndex });

	while (!openList.empty()) {
		int currentIndex = openList.top().second;
		openList.pop();

		if (currentIndex == goalIndex)break;

		if (nodeRecords[currentIndex].isClosed)continue;
		nodeRecords[currentIndex].isClosed = false;

		for (int neighborIndex : mapnode[currentIndex].connectedNode) {
			if (nodeRecords[neighborIndex].isClosed)continue;

			float distToNeighbor = GetDistance(mapnode[currentIndex].position, mapnode[neighborIndex].position);
			float tentative_costG = nodeRecords[currentIndex].costG + distToNeighbor;

			if (tentative_costG < nodeRecords[neighborIndex].costG) {
				nodeRecords[neighborIndex].parentID = currentIndex;
				nodeRecords[neighborIndex].costG = tentative_costG;

				float costH = GetDistance(mapnode[neighborIndex].position, mapnode[goalIndex].position);

				nodeRecords[neighborIndex].costF = tentative_costG + costH;

				openList.push({ nodeRecords[neighborIndex].costF,neighborIndex });
			}
		}
	}

	if (nodeRecords[goalIndex].parentID != -1 || startIndex == goalIndex) {
		int curr = goalIndex;
		while (curr != -1) {
			path.push_back(mapnode[curr].position);
			curr = nodeRecords[curr].parentID;
		}

		std::reverse(path.begin(), path.end());
	}

	return path;
	
}