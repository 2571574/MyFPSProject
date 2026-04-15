#include "Camera.h"
#include "Debug.h"
#include "Time.h"
#include "ConfigManager.h"
#include "Param/Global.h"
#include "Param/System.h"

#include <cmath>

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
	float dt60 = Global::Math::FPS_BASE * dt;

	//カメラの操作入力を得る
	int mouseX, mouseY, stickX, stickY;
	CheckKey::GetIns().GetMousePosition(mouseX, mouseY);
	CheckKey::GetIns().GetRightStick(stickX, stickY);

	//ConfigManagerより設定の取得
	float mouseSens = ConfigManager::GetIns().Settings().mouseSensitivity;
	float padSens = ConfigManager::GetIns().Settings().padSensitivity;
	recovery = ConfigManager::GetIns().Settings().recovery;

	//マウス
	int deltaX = System::Window::CENTER_X - mouseX;
	int deltaY = System::Window::CENTER_Y - mouseY;
	yaw -= deltaX * mouseSens;
	pitch += deltaY * mouseSens;
	SetMousePoint(System::Window::CENTER_X, System::Window::CENTER_Y);

	//コントローラー　
	yaw += stickX * padSens * dt60;
	pitch -= stickY * padSens * dt60;

	//リコイルの追加
	float moveY = pendingRecoilYaw * System::Camera::RECOIL_SPEED * dt;
	float moveP = pendingRecoilPitch * System::Camera::RECOIL_SPEED * dt;

	yaw += moveY;
	pitch += moveP;

	pendingRecoilYaw -= moveY;
	pendingRecoilPitch -= moveP;
	//適用
	if (recovery) {
		recoilYaw += moveY;
		recoilPitch += moveP;

		float recY = recoilYaw * System::Camera::RECOVERY_SPEED * dt;
		float recP = recoilPitch * System::Camera::RECOVERY_SPEED * dt;

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
	if (pitch > System::Camera::CAM_ANGLESNAP_PITCH) pitch = System::Camera::CAM_ANGLESNAP_PITCH;
	if (pitch < -System::Camera::CAM_ANGLESNAP_PITCH) pitch = -System::Camera::CAM_ANGLESNAP_PITCH;

	if (yaw > System::Camera::CAM_ANGLESNAP_YAW) yaw -= System::Camera::YAW_SNAP;
	if (yaw < -System::Camera::CAM_ANGLESNAP_YAW) yaw += System::Camera::YAW_SNAP;
}

void Camera::AddAngle(float deltaYaw, float deltaPitch){
	pendingRecoilYaw += deltaYaw;
	pendingRecoilPitch += deltaPitch;
}

void Camera::SetAngle(float newYaw, float newPitch) {
	yaw = newYaw;
	pitch = newPitch;
}
void Camera::Move(float fov) {
	float camYaw = yaw;
	float camPitch = pitch;
	//注視点を計算
	VECTOR targetPos;
	targetPos.x = camPos.x + cosf(camPitch * Global::Math::DEG_TO_RAD) * sinf(camYaw * Global::Math::DEG_TO_RAD);
	targetPos.y = camPos.y + sinf(camPitch * Global::Math::DEG_TO_RAD);
	targetPos.z = camPos.z + cosf(camPitch * Global::Math::DEG_TO_RAD) * cosf(camYaw * Global::Math::DEG_TO_RAD);
	SetupCamera_Perspective(fov);
	SetCameraPositionAndTarget_UpVecY(camPos, targetPos);
}


void Camera::SetPos(VECTOR pos) {
	camPos = pos;
}
/*プレイヤーから見た前のベクトルの方向を得る*/
void Camera::GetForwardVec(VECTOR& forward, VECTOR& right) const {
	forward = VGet(sinf(yaw * Global::Math::DEG_TO_RAD), 0.0f, cosf(yaw * Global::Math::DEG_TO_RAD));
	right = VGet(forward.z, 0.0f, -forward.x);
}

/*カメラの向いている方向を前としたベクトルを得る*/
VECTOR Camera::GetLookDirection()const {
	VECTOR out;
	float camYaw = yaw;
	float camPitch = pitch;
	float radY = camYaw * Global::Math::DEG_TO_RAD;
	float radP = camPitch * Global::Math::DEG_TO_RAD;
	float cp = cosf(radP);
	out.x = cp * sinf(radY);
	out.y = sinf(radP);
	out.z = cp * cosf(radY);
	out = VNorm(out);
	return out;
}