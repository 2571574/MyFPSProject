#include "Pathfinding.h"

#include <queue>
#include<unordered_map>
#include<algorithm>
#include<cfloat>

namespace {
	constexpr float GRID_SIZE = 2.0f;
	constexpr float CHARA_HEIGHT = 2.0f;
	constexpr float CHARA_RADIUS = 0.5f;

	constexpr float MAP_START_Y = 30.0f;
	constexpr float MAP_END_Y = -10.0f;
	constexpr float MAX_SLOPE_NORMAL = 0.6f;

	constexpr float MAP_MIN_X = -30.0f;
	constexpr float MAP_MAX_X = 30.0f;
	constexpr float MAP_MIN_Z = -30.0f;
	constexpr float MAP_MAX_Z = 30.0f;

	constexpr float RAY_Y_OFFSET = 1.0f;
	constexpr float GROUND_CHECK_DIST = 0.4f;

	//ノード探索のための情報
	struct NodeRecord {
		bool isClosed = false;
		int parentID = -1;
		float costF = FLT_MAX;
		float costG = FLT_MAX;

		bool operator>(const NodeRecord& other) const {
			return costF > other.costF;
		}
	};
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
}

void InitNode(int modelhandle, std::vector<Node>& mapnode) {
	mapnode.clear();

	for (float x =MAP_MIN_X; x <= MAP_MAX_X; x += GRID_SIZE) {
		for (float z = MAP_MIN_Z; z <= MAP_MAX_Z; z += GRID_SIZE) {
			VECTOR rayStart = VGet(x, MAP_START_Y, z);
			VECTOR rayEnd = VGet(x, MAP_END_Y, z);


			while (true) {
				MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(modelhandle, -1, rayStart, rayEnd);
				if (hit.HitFlag == 0) {
					break;
				}

				if (IsSafePlace(modelhandle, hit.HitPosition, CHARA_RADIUS, CHARA_HEIGHT, MAX_SLOPE_NORMAL)) {
					Node node;
					node.position = VAdd(hit.HitPosition, VGet(0.0f, 0.1f, 0.0f));
					mapnode.push_back(node);
				}

				rayStart = VAdd(hit.HitPosition, VGet(0.0f, -0.5f, 0.0f));
				if (rayStart.y < rayEnd.y)break;


			}
		}

	}

	float connectDistMax = GRID_SIZE * 1.8f;

	for (size_t i = 0; i < mapnode.size(); ++i) {
		for (size_t j = i + 1; j < mapnode.size(); ++j) {
			VECTOR posA = mapnode[i].position;
			VECTOR posB = mapnode[j].position;

			float dist = GetDistance(posA, posB);
			if (dist > connectDistMax) continue;

			float walkableTan = sqrtf(1.0f / (MAX_SLOPE_NORMAL * MAX_SLOPE_NORMAL) - 1.0f);
			float maxWalkableDiff = dist * walkableTan + 0.2f;
			if (std::abs(posA.y - posB.y) > maxWalkableDiff)continue;

			VECTOR capStart = VAdd(posA, VGet(0, CHARA_HEIGHT * 0.5f, 0));
			VECTOR capEnd = VAdd(posB, VGet(0, CHARA_HEIGHT * 0.5f, 0));

			MV1_COLL_RESULT_POLY_DIM wallHit = MV1CollCheck_Capsule(modelhandle, -1, capStart, capEnd, CHARA_RADIUS);
			bool isBlockedByWall = false;
			if (wallHit.HitNum > 0) {
				for (int k = 0; k < wallHit.HitNum; k++) {
					if (wallHit.Dim[k].Normal.y < MAX_SLOPE_NORMAL) {
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

				if (groundHit.HitFlag == 0 || groundHit.Normal.y < MAX_SLOPE_NORMAL) {
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
int GetNearestNodeIndex(VECTOR pos, const std::vector<Node>& mapnode) {
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

VECTOR GetNodePosition(int nodeID, const std::vector<Node>& mapnode) {
	if (nodeID >= 0 && nodeID < mapnode.size()) {
		return mapnode[nodeID].position;
	}
	return VGet(0.0f, 0.0f, 0.0f);
}

float GetDistance(VECTOR a, VECTOR b) { return VSize(VSub(a, b)); };

std::vector<VECTOR>FindPath(VECTOR startPos, VECTOR goalPos, const std::vector<Node>&mapnode) {
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