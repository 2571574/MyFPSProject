#pragma once
#include <vector>
#include "DxLib.h"

//ノードの基本情報
struct Node {
	VECTOR position;
	std::vector <int> connectedNode;
};

/// <summary>
/// ノードの初期化　マップ上にノードを配置し、繋げる
/// </summary>
/// <param name="modelhandle">ステージのモデルハンドル</param>
/// <param name="mapnode">配置したノード参照</param>
/// <param name="pair">ペア参照</param>
void InitNode(int modelhandle, std::vector<Node>& mapnode);

/// <summary>
/// スタート地点からゴール地点までの最短経路をノードで求める
/// </summary>
/// <param name="startPos">スタート地点</param>
/// <param name="goalPos">ゴール地点</param>
/// <param name="mapnode">ノード情報</param>
/// <returns>通るノードの位置の配列</returns>
std::vector<VECTOR> FindPath(VECTOR startPos, VECTOR goalPos, const std::vector<Node>& mapnode);

//getter

VECTOR GetNodePosition(int nodeID, const std::vector<Node>& mapnode);
float GetDistance(VECTOR a, VECTOR b);
int GetNearestNodeIndex(VECTOR pos, const std::vector<Node>& mapnode);