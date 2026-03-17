#include "Camera.h"
#include "Debug.h"
#include "Time.h"
#include "ConfigManager.h"

#include <cmath>

namespace {
	constexpr float CAM_ANGLESNAP_PITCH = 89.9f;		//カメラのy軸制限
	constexpr float CAM_ANGLESNAP_YAW = 180.0f;		//カメラのx軸制限
	constexpr float YAW_SNAP = 360.0f;				//x軸のスナップ用
	constexpr float RECOIL_SPEED = 15.0f;
	constexpr float RECOVERY_SPEED = 3.0f;
}

/*コンストラクタ*/
Camera::Camera() 
	: camPos(VGet(0.0f, 0.0f, 0.0f)) 
	, yaw(0.0f)
	, pitch(0.0f)
	, recoilYaw(0.0f)
	, recoilPitch(0.0f)
	, pendingRecoilYaw(0.0f)
	, pendingRecoilPitch(0.0f){
}

void Camera::Update(VECTOR Pos) {

	//座標を得る
	camPos = Pos;
	float dt = Time::GetIns().GetDelta();
	float dt60 = 60.0f * dt;

	//カメラの操作入力を得る
	int mouseX, mouseY, stickX, stickY;
	CheckKey::GetIns().GetMousePosition(mouseX, mouseY);
	CheckKey::GetIns().GetRightStick(stickX, stickY);

	//ConfigManagerより設定の取得
	float mouseSens = ConfigManager::GetIns().Settings().mouseSensitivity;
	float padSens = ConfigManager::GetIns().Settings().padSensitivity;
	recovery = ConfigManager::GetIns().Settings().recovery;

	//マウス
	int deltaX = CENTER_X - mouseX;
	int deltaY = CENTER_Y - mouseY;
	yaw -= deltaX * mouseSens;
	pitch += deltaY * mouseSens;
	SetMousePoint(CENTER_X, CENTER_Y);

	//コントローラー　
	yaw += stickX * padSens * dt60;
	pitch -= stickY * padSens * dt60;

	//リコイルの追加
	float moveY = pendingRecoilYaw * RECOIL_SPEED * dt;
	float moveP = pendingRecoilPitch * RECOIL_SPEED * dt;

	yaw += moveY;
	pitch += moveP;

	pendingRecoilYaw -= moveY;
	pendingRecoilPitch -= moveP;
	//適用
	if (recovery) {
		recoilYaw += moveY;
		recoilPitch += moveP;

		float recY = recoilYaw * RECOVERY_SPEED * dt;
		float recP = recoilPitch * RECOVERY_SPEED * dt;

		yaw -= recY;
		pitch -= recP;
		recoilYaw -= recY;
		recoilPitch -= recP;
	}
	else {
		recoilYaw = 0.0f;
		recoilPitch = 0.0f;
	}

	//補正
	if (pitch > CAM_ANGLESNAP_PITCH)pitch = CAM_ANGLESNAP_PITCH;
	if (pitch < -CAM_ANGLESNAP_PITCH)pitch = -CAM_ANGLESNAP_PITCH;

	if (yaw > CAM_ANGLESNAP_YAW)yaw -= YAW_SNAP;
	if (yaw < -CAM_ANGLESNAP_YAW)yaw += YAW_SNAP;

}

void Camera::AddAngle(float deltaYaw, float deltaPitch){
	pendingRecoilYaw += deltaYaw;
	pendingRecoilPitch += deltaPitch;
}

void Camera::Move(float fov) {
	float camYaw = yaw;
	float camPitch = pitch;
	//注視点を計算
	VECTOR targetPos;	//カメラの注視点
	targetPos.x = camPos.x + cosf(camPitch*(DX_PI_F/180)) * sinf(camYaw*(DX_PI_F/180));
	targetPos.y = camPos.y + sinf(camPitch*(DX_PI_F/180));
	targetPos.z = camPos.z + cosf(camPitch*(DX_PI_F/180)) * cosf(camYaw*(DX_PI_F/180));
	SetupCamera_Perspective(fov);
	SetCameraPositionAndTarget_UpVecY(camPos, targetPos);	//カメラ位置と注視点をセット
}


void Camera::SetPos(VECTOR pos) {
	camPos = pos;
}
/*プレイヤーから見た前のベクトルの方向を得る*/
void Camera::GetForwardVec(VECTOR& forward, VECTOR& right) const {
	forward = VGet(sinf(yaw * (DX_PI_F / 180)), 0, cosf(yaw * (DX_PI_F / 180)));
	right = VGet(forward.z, 0, -forward.x);
}

/*カメラの向いている方向を前としたベクトルを得る*/
VECTOR Camera::GetLookDirection()const{
	VECTOR out;
	float camYaw = yaw;
	float camPitch = pitch;
	float radY = camYaw * (DX_PI_F / 180.0f);
	float radP = camPitch * (DX_PI_F / 180.0f);
	float cp = cosf(radP);
	out.x = cp * sinf(radY);
	out.y = sinf(radP);
	out.z = cp * cosf(radY);
	out = VNorm(out);
	return out;
}
