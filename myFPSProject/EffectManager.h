#pragma once
#include <vector>
#include <memory>
#include "Effect.h"
#include "Status.h"

/// <summary>
/// エフェクトを管理するマネージャークラス
/// </summary>
class EffectManager
{
private:
	std::vector<std::unique_ptr<Effect>> effects;	//エフェクトのリスト
	EffectManager() = default;

public:
	static EffectManager& GetIns();

	/// <summary>
	/// エフェクトを生成する
	/// </summary>
	/// <param name="effect">生成するエフェクト</param>
	void AddEffect(std::unique_ptr<Effect> effect);
	void Update();
	void Draw();
	void Clear();

	/// <summary>
	/// 敵の死亡時の破散エフェクト
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="floorY">床の高さ</param>
	/// <param name="type">敵の種類</param>
	void CreateDeathParticle(VECTOR pos, float floorY, ENEMYTYPE type);

	/// <summary>
	/// 銃のマズルフラッシュ
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="dir">方向</param>
	/// <param name="size">大きさ</param>
	void CreateMuzzleFlash(VECTOR pos, VECTOR dir, float size);

	/// <summary>
	/// 着弾エフェクト
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="normal">着弾点の法線ベクトル</param>
	/// <param name="isEnemy">敵に当たったかどうか</param>
	void CreateHitEffect(VECTOR pos, VECTOR normal, bool isEnemy);

	/// <summary>
	/// 敵のスポーンエフェクト
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="height">エフェクトの最大高さ</param>
	/// <param name="radius">半径</param>
	/// <param name="life">描画時間</param>
	void CreateSpawnEffect(VECTOR pos, float height, float radius, float life);

	/// <summary>
	/// 爆発エフェクト
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="radius">半径</param>
	/// <param name="color">色</param>
	void CreateExplosionEffect(VECTOR pos, float radius, int color);

	/// <summary>
	/// ヒットスキャンの弾のトレイル
	/// </summary>
	/// <param name="start">始点</param>
	/// <param name="end">終点</param>
	/// <param name="color">色</param>
	void CreateHitScanTrail(VECTOR start, VECTOR end, int color);

	/// <summary>
	/// プロジェクタイルの弾のトレイル
	/// </summary>
	/// <param name="start">始点</param>
	/// <param name="end">終点</param>
	/// <param name="radius">半径</param>
	/// <param name="color">色</param>
	void CreateProjectileTrail(VECTOR start, VECTOR end, float radius, int color);
};

