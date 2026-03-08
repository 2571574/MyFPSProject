#include "Pathfinding.h"
#include <queue>
#include<unordered_map>
#include<algorithm>
#include "Debug.h"
void InitNode(int modelhandle, std::vector<Node>& mapnode, std::vector<connectNodepair> & pair) {
	float gridSize = 2.0f;
	float startY = 30.0f;
	float endY = -20.0f;

	float maxSlopeNormal = 0.6f;
	float charaHeight = 2.0f;
	float charaRadius = 0.5f;
	float maxStepHeight = 2.0f;

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

				if (hit.Normal.y >= maxSlopeNormal) {
					bool isValidNode = true;
					VECTOR pos = hit.HitPosition;

					float CheckDist = charaRadius + 0.1f;
					VECTOR offsets[4] = { VGet(CheckDist,0,0),VGet(-CheckDist,0,0),VGet(0,0,CheckDist),VGet(0,0,-CheckDist) };

					for (int i = 0; i < 4; i++) {
						VECTOR origin = VAdd(pos, offsets[i]);

						VECTOR edgeStart = VGet(origin.x, pos.y + maxStepHeight, origin.z);
						VECTOR edgeEnd = VGet(origin.x, pos.y - maxStepHeight, origin.z);

						MV1_COLL_RESULT_POLY edgeHit = MV1CollCheck_Line(modelhandle, -1, edgeStart, edgeEnd);

						if (edgeHit.HitFlag == 0 || edgeHit.Normal.y<maxSlopeNormal || std::abs(edgeHit.HitPosition.y - pos.y) > maxStepHeight) {
							isValidNode = false;
							break;
						}
					}

					if(isValidNode){
						VECTOR checkBottom = VGet(hit.HitPosition.x, hit.HitPosition.y + charaRadius + 0.1f, hit.HitPosition.z);
						VECTOR checkTop = VGet(hit.HitPosition.x, hit.HitPosition.y + charaHeight - charaRadius, hit.HitPosition.z);
						MV1_COLL_RESULT_POLY_DIM spaceHit = MV1CollCheck_Capsule(modelhandle, -1, checkBottom, checkTop, charaRadius);
						if (spaceHit.HitNum != 0) {
							for (int i = 0; i < spaceHit.HitNum; i++) {
								if (spaceHit.Dim[i].Normal.y < maxSlopeNormal) {
									isValidNode = false;
									break;
								}
							}
						}
						MV1CollResultPolyDimTerminate(spaceHit);

						if (isValidNode) {
							Node node;
							node.position = VAdd(hit.HitPosition, VGet(0.0f, 0.1f, 0.0f));
							mapnode.push_back(node);
						}
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
			bool hitWall = false;
			if (capHit.HitNum != 0) {
				for (int j = 0; j < capHit.HitNum; j++) {
					if (capHit.Dim[j].Normal.y < maxSlopeNormal) {
						hitWall = true;
						break;
					}
				}
				MV1CollResultPolyDimTerminate(capHit);
				if (hitWall)continue;
			}
			float distXZ = VSize(VGet(posA.x - posB.x, 0.0f, posA.z - posB.z));
			int divCount = (int)(distXZ / 0.5f);
			if (divCount < 1)divCount = 1;

			bool isPathValid = true;
			float prevGroundY = posA.y;

			float stepDist = distXZ / (divCount + 1);

			float maxWalkableStep = stepDist * sqrtf(1.0f / (maxSlopeNormal * maxSlopeNormal) - 1.0f)  + 0.05f;

			for (int d = 1; d <= divCount; d++) {
				float t = (float)d / (divCount + 1);

				float px = posA.x + (posB.x - posA.x) * t;
				float pz = posA.z + (posB.z - posA.z) * t;

				VECTOR rayStart = VGet(px, prevGroundY + maxWalkableStep, pz);
				VECTOR rayEnd = VGet(px, prevGroundY - maxWalkableStep, pz);

				MV1_COLL_RESULT_POLY groundHit = MV1CollCheck_Line(modelhandle, -1, rayStart, rayEnd);
				if (groundHit.HitFlag == 0) {
					isPathValid = false;
					break;
				}

				if (groundHit.Normal.y < maxSlopeNormal) {
					isPathValid = false;
					break;
				}

				float heightDiff = std::abs(groundHit.HitPosition.y - prevGroundY);
				if (heightDiff > maxStepHeight) {
					isPathValid = false;
					break;
				}

				prevGroundY = groundHit.HitPosition.y;
			}
			if (std::abs(posB.y - prevGroundY) > maxStepHeight) {
				isPathValid = false;
			}

			if (!isPathValid) continue;

			mapnode[i].connectedNode.push_back(j);
			mapnode[j].connectedNode.push_back(i);
			connectNodepair paircon;
			paircon.pos1 = mapnode[i].position;
			paircon.pos2 = mapnode[j].position;
			
			pair.push_back(paircon);
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