#pragma once
#include <vector>
#include<memory>
#include "BaseProjectile.h"

/// <summary>
/// プロジェクタイルの弾全体のマネージャークラス
/// </summary>
class ProjectileManager
{
private:
	ProjectileManager() = default;
	std::vector<std::unique_ptr<BaseProjectile>> projectiles;	//弾の配列
public:
	static ProjectileManager& GetIns();

	/// <summary>
	/// 弾を生成する
	/// </summary>
	/// <param name="b">生成する弾の情報</param>
	void Spawn(std::unique_ptr<BaseProjectile> b);

	
	void Update();
	void Draw();

	/// <summary>
	/// 存在する弾を全て消去する
	/// </summary>
	void Clear();

	const std::vector<std::unique_ptr<BaseProjectile>>& GetBullets()const {
		return projectiles;
	}
};
