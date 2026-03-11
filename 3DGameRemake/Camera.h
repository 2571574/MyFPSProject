#pragma once
#include "DxLib.h"
#include "CheckKey.h"
/// <summary>
/// カメラを管理するクラス
/// </summary>
class Camera
{
private:
	float yaw;		//水平
	float pitch;	//垂直
	float pendingRecoilYaw;	//射撃反動の保持　水平
	float pendingRecoilPitch;//射撃反動の保持　垂直

	VECTOR camPos;	//カメラの座標

	float recoilYaw;	//反動によるカメラの移動　水平
	float recoilPitch;  //反動によるカメラの移動　垂直
	bool recovery;		//反動を徐々に回復させるか

public:
	Camera();	//コンストラクタ
	
	/// <summary>
	/// 入力を得て、カメラの位置と向きを更新する関数
	/// </summary>
	/// <param name="Pos">カメラの位置</param>
	void Update(VECTOR Pos);

	/// <summary>
	/// カメラに角度を加算する　（反動を加えるときなどに使用）
	/// </summary>
	/// <param name="deltaYaw">水平加算量</param>
	/// <param name="deltaPitch">垂直加算量</param>
	void AddAngle(float deltaYaw, float deltaPitch);

	/// <summary>
	/// 位置と向きの更新を反映させる
	/// </summary>
	/// <param name="fov">視野角</param>
	void Move(float fov);

	//getter
	void SetPos(VECTOR pos);
	void GetForwardVec(VECTOR& forward, VECTOR& right);
	VECTOR GetLookDirection();
	float GetYaw()const { return yaw; }
};

