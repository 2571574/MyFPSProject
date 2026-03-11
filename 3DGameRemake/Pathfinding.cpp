#include "Pathfinding.h"
#include <queue>
#include<unordered_map>
#include<algorithm>
#include "Debug.h"
bool IsSafePlace(int modelhandle, VECTOR pos, float charaRadius, float charaHeight, float maxSlope) {
	VECTOR rayStart = VAdd(pos, VGet(0.0f, 1.0f, 0.0f));
	VECTOR rayEnd = VAdd(pos, VGet(0.0f, -1.0f, 0.0f));
	MV1_COLL_RESULT_POLY centerHit = MV1CollCheck_Line(modelhandle, -1, rayStart, rayEnd);

	if (centerHit.HitFlag == 0 || centerHit.Normal.y < maxSlope)return false;

	float checkDist = charaRadius;
	for (int i = 0; i < 8; i++) {
		float angle = i * (DX_PI_F / 4);

		VECTOR offset = VGet(cosf(angle) * checkDist, 0, sinf(angle) * checkDist);
		VECTOR p = VAdd(centerHit.HitPosition, offset);

		VECTOR s = VAdd(p, VGet(0.0f, 1.0f, 0.0f));
		VECTOR e = VAdd(p, VGet(0.0f, -1.0f, 0.0f));
		MV1_COLL_RESULT_POLY edgeHit = MV1CollCheck_Line(modelhandle, -1, s, e);

		if (edgeHit.HitFlag == 0 || std::abs(edgeHit.HitPosition.y - centerHit.HitPosition.y) > 0.5f) {
			return false;
		}
	}

	VECTOR capBottom = VAdd(centerHit.HitPosition, VGet(0, charaRadius + 0.1f, 0));
	VECTOR capTop = VAdd(centerHit.HitPosition, VGet(0, charaHeight - charaRadius, 0));

	MV1_COLL_RESULT_POLY_DIM spaceHit = MV1CollCheck_Capsule(modelhandle, -1, capBottom, capTop,charaRadius);
	bool hasSpace = (spaceHit.HitNum == 0);
	MV1CollResultPolyDimTerminate(spaceHit);

	return hasSpace;
}

void InitNode(int modelhandle, std::vector<Node>& mapnode) {
	mapnode.clear();

	float gridSize = 2.0f;
	float charaHeight = 2.0f;
	float charaRadius = 0.5f;

	float startY = 30.0f;
	float endY = -10.0f;
	float maxSlopeNormal = 0.6f;
	float maxStepHeight = 1.5f;

	float minX = -30.0f;	float maxX = 30.0f;
	float minZ = -30.0f;	float maxZ = 30.0f;

	for (float x = minX; x <= maxX; x += gridSize) {
		for (float z = minZ; z <= maxZ; z += gridSize) {
			VECTOR rayStart = VGet(x, startY, z);
			VECTOR rayEnd = VGet(x, endY, z);


			while (1) {
				MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(modelhandle, -1, rayStart, rayEnd);
				if (hit.HitFlag == 0) {
					break;
				}

				if (IsSafePlace(modelhandle, hit.HitPosition, charaRadius, charaHeight, maxSlopeNormal)) {
					Node node;
					node.position = VAdd(hit.HitPosition, VGet(0.0f, 0.1f, 0.0f));
					mapnode.push_back(node);
				}

				rayStart = VAdd(hit.HitPosition, VGet(0.0f, -0.5f, 0.0f));
				if (rayStart.y < rayEnd.y)break;


			}
		}

	}

	float connectDistMax = gridSize * 1.8f;

	for (size_t i = 0; i < mapnode.size(); ++i) {
		for (size_t j = i + 1; j < mapnode.size(); ++j) {
			VECTOR posA = mapnode[i].position;
			VECTOR posB = mapnode[j].position;

			float dist = GetDistance(posA, posB);
			if (dist > connectDistMax) continue;

			float walkableTan = sqrtf(1.0f / (maxSlopeNormal * maxSlopeNormal) - 1.0f);
			float maxWalkableDiff = dist * walkableTan + 0.2f;
			if (std::abs(posA.y - posB.y) > maxWalkableDiff)continue;

			VECTOR capStart = VAdd(posA, VGet(0, charaHeight * 0.5f, 0));
			VECTOR capEnd = VAdd(posB, VGet(0, charaHeight * 0.5f, 0));

			MV1_COLL_RESULT_POLY_DIM wallHit = MV1CollCheck_Capsule(modelhandle, -1, capStart, capEnd, charaRadius);
			bool isBlockedByWall = false;
			if (wallHit.HitNum > 0) {
				for (int k = 0; k < wallHit.HitNum; k++) {
					if (wallHit.Dim[k].Normal.y < maxSlopeNormal) {
						isBlockedByWall = true;
						break;
					}
				}
			}

			MV1CollResultPolyDimTerminate(wallHit);
			if (isBlockedByWall)continue;

			bool isGroundContinuous = true;
			int div = (int)(dist / 0.4f);
			if (div < 1)div = 1;

			float prevY = posA.y;
			for (int k = 1; k <= div; k++) {
				float t = (float)k / (div + 1);
				VECTOR checkPos = VAdd(posA, VScale(VSub(posB, posA), t));
				
				VECTOR s = VGet(checkPos.x, prevY + 0.5f, checkPos.z);
				VECTOR e = VGet(checkPos.x, prevY - 0.5f, checkPos.z);
				MV1_COLL_RESULT_POLY groundHit = MV1CollCheck_Line(modelhandle, -1, s, e);

				if (groundHit.HitFlag == 0 || groundHit.Normal.y < maxSlopeNormal) {
					isGroundContinuous = false;
					break;
				}
				prevY = groundHit.HitPosition.y;
			}

			if (isGroundContinuous) {
				mapnode[i].connectedNode.push_back(j);
				mapnode[j].connectedNode.push_back(i);
			}
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
		nodeRecords[currentIndex].isClosed = true;

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