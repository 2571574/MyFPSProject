#pragma once
#include "DxLib.h"
#include "CheckKey.h"
/*カメラを管理する関数*/
class Camera
{
private:
	float yaw;		//水平
	float pitch;	//垂直
	float pendingRecoilYaw;
	float pendingRecoilPitch;

	VECTOR camPos;	//カメラの座標

	float recoilYaw;
	float recoilPitch;
	bool recovery;

public:
	Camera();	//コンストラクタ
	void Update(VECTOR Pos);	//更新
	void SetPos(VECTOR pos);	//座標をセット
	void Move(float fov);	//反映

	//getter
	void GetForwardVec(VECTOR& forward, VECTOR& right);
	VECTOR GetLookDirection();

	void AddAngle(float deltaYaw, float deltaPitch);
};

