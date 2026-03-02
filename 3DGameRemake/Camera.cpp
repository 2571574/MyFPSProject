#include "Camera.h"
#include "math.h"
#include "Debug.h"
#include "Time.h"
/*コンストラクタ*/
Camera::Camera() :yaw(0.0f), pitch(0.0f),pendingRecoilYaw(0.0f), pendingRecoilPitch(0.0f), recoilYaw(0.0f), recoilPitch(0.0f), camPos(VGet(0.0f, 0.0f, 0.0f)),recovery(true) {
}

/*カメラの更新*/
void Camera::Update(VECTOR Pos) {
	//座標を得る
	camPos = Pos;
	float dt = Time::GetIns().GetDelta();
	float dt60 = 60.0f * dt;
	//カメラの操作入力を得る
	int mouseX, mouseY, stickX, stickY;
	CheckKey::GetIns().GetMousePosition(mouseX, mouseY);
	CheckKey::GetIns().GetRightStick(stickX, stickY);
	//マウス
	int deltaX = CENTER_X - mouseX;
	int deltaY = CENTER_Y - mouseY;
	yaw -= deltaX * SENSITIVITY * dt60;
	pitch += deltaY * SENSITIVITY * dt60;
	SetMousePoint(CENTER_X, CENTER_Y);

	//コントローラー　
	yaw += stickX * PAD_SENSITIVITY * dt60;
	pitch -= stickY * PAD_SENSITIVITY * dt60;

	float moveY = pendingRecoilYaw * RECOIL_SPEED * dt;
	float moveP = pendingRecoilPitch * RECOIL_SPEED * dt;

	recoilYaw += moveY;
	recoilPitch += moveP;
	pendingRecoilYaw -= moveY;
	pendingRecoilPitch -= moveP;

	if (recovery) {
		recoilYaw -= recoilYaw * RECOVERY_SPEED * dt;
		recoilPitch -= recoilPitch * RECOVERY_SPEED * dt;
	}
	//補正
	if (pitch > CAM_ANGLESNAP_PITCH)pitch = CAM_ANGLESNAP_PITCH;
	if (pitch < -CAM_ANGLESNAP_PITCH)pitch = -CAM_ANGLESNAP_PITCH;

	if (yaw > CAM_ANGLESNAP_YAW)yaw -= YAW_SNAP;
	if (yaw < -CAM_ANGLESNAP_YAW)yaw += YAW_SNAP;

}
void Camera::SetPos(VECTOR pos) {
	camPos = pos;
}
/*カメラの更新を反映*/
void Camera::Move(float fov) {
	float camYaw = yaw + recoilYaw;
	float camPitch = pitch + recoilPitch;
	//注視点を計算
	VECTOR targetPos;	//カメラの注視点
	targetPos.x = camPos.x + cosf(camPitch*(DX_PI_F/180)) * sinf(camYaw*(DX_PI_F/180));
	targetPos.y = camPos.y + sinf(camPitch*(DX_PI_F/180));
	targetPos.z = camPos.z + cosf(camPitch*(DX_PI_F/180)) * cosf(camYaw*(DX_PI_F/180));
	SetupCamera_Perspective(fov);
	SetCameraPositionAndTarget_UpVecY(camPos, targetPos);	//カメラ位置と注視点をセット
}

/*プレイヤーから見た前のベクトルの方向を得る*/
void Camera::GetForwardVec(VECTOR& forward, VECTOR& right) {
	forward = VGet(sinf(yaw * (DX_PI_F / 180)), 0, cosf(yaw * (DX_PI_F / 180)));
	right = VGet(forward.z, 0, -forward.x);
}

/*カメラの向いている方向を前としたベクトルを得る*/
VECTOR Camera::GetLookDirection(){
	VECTOR out;
	float camYaw = yaw + recoilYaw;
	float camPitch = pitch + recoilPitch;
	float radY = camYaw * (DX_PI_F / 180.0f);
	float radP = camPitch * (DX_PI_F / 180.0f);
	float cp = cosf(radP);
	out.x = cp * sinf(radY);
	out.y = sinf(radP);
	out.z = cp * cosf(radY);
	out = VNorm(out);
	return out;
}

void Camera::AddAngle(float deltaYaw, float deltaPitch){
	pendingRecoilYaw += deltaYaw;
	pendingRecoilPitch += deltaPitch;
}