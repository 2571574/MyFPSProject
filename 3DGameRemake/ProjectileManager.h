#pragma once
#include <vector>
#include<memory>
#include "BaseProjectile.h"

/*弾速のある弾の実体を管理するクラス*/
class ProjectileManager
{
private:
	ProjectileManager() = default;	//コンストラクタ
	std::vector<std::unique_ptr<BaseProjectile>> projectiles;	//弾の配列
public:
	static ProjectileManager& GetIns();		//インスタンスを取得

	void Spawn(std::unique_ptr<BaseProjectile> b);		//弾の生成
	void Update();		//更新
	void Draw();		//描画
	void Clear();		//全消去

	//getter
	const std::vector<std::unique_ptr<BaseProjectile>>& GetBullets()const {
		return projectiles;
	}
};
