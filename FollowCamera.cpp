#include "FollowCamera.h"
#include<imgui.h>

void FollowCamera::Initialize() { 
	viewProjection_.farZ = 2000.0f;
	viewProjection_.nearZ = 0.01f;
	viewProjection_.Initialize();
	destinationAngleX_= 0.2f;

	distance = -15.0f;
	
	shotOffset = {2.0f, 6.0f, -7.5f};

	rootOffset = {0.0f, 6.0f, distance};

	minRotate = -0.31f;
	maxRotate = 0.9f;

	baseOffset = rootOffset;

	vector_ = std::make_unique<MyVector>();
	matrix_ = std::make_unique<MyMatrix>();

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void FollowCamera::SetTarget(const WorldTransform* target) { 
	target_ = target;
	Reset();

}

void FollowCamera::Update() {

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float rotateSpeed = 0.05f;

		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
			/*if (distance <= -10.0f && distance >= -100.0f){
				distance += (float)joyState.Gamepad.sThumbRY / SHRT_MAX;
			} */		
			
		} else {
			destinationAngleY_ += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * rotateSpeed;

			if (destinationAngleX_>-1.57f&&destinationAngleX_<1.57f) {
				destinationAngleX_ -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * rotateSpeed;
			}
			
		}
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			destinationAngleY_ = target_->rotation_.y;
			destinationAngleX_ = 0.2f;
		}
		if (joyState.Gamepad.bLeftTrigger!=0) {
			t = 1.0f;
			maxRotate = 0.4f;
			baseOffset = vector_->Lerp(baseOffset, shotOffset, offset_t);
		} else {
			t = 0.1f;
			maxRotate = 0.9f;
			baseOffset = vector_->Lerp(baseOffset, rootOffset, offset_t);
		}
	}
	if (input_->TriggerKey(DIK_R)) {
		destinationAngleY_ = target_->rotation_.y;
		destinationAngleX_ = 0.2f;
	}
	/*if (distance <= -10.0f && distance >= -100.0f) {
		if (input_->PushKey(DIK_E)) {
			distance += 1.0f;
		} else if (input_->PushKey(DIK_Q)) {
			distance -= 1.0f;
		}
	} */

	if (distance < -100.0f) {
		distance = -100.0f;
	} else if (distance > -10.0f) {
		distance = -10.0f;
	}

	if (destinationAngleX_ <= minRotate) {
		destinationAngleX_ = minRotate;
	} else if (destinationAngleX_ >= maxRotate) {
		destinationAngleX_ = maxRotate;
	}
	
	viewProjection_.rotation_.y =
	    vector_->LerpShortAngle(viewProjection_.rotation_.y, destinationAngleY_, 0.1f);
	viewProjection_.rotation_.x =
	    vector_->LerpShortAngle(viewProjection_.rotation_.x, destinationAngleX_, 0.1f);

	

	if (target_) {
		//追従座標の補完
		interTarget_ = vector_->Lerp(interTarget_, target_->translation_, t);
		//追従対象からカメラまでのオフセット
		Vector3 offset = offsetCalculation(baseOffset);

		//座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = interTarget_ + offset;

	}
	

	

	viewingFrustum_ = {
	    .translation_ = viewProjection_.translation_,
	    .rotate_ = viewProjection_.rotation_,
	    .direction = {0.0f, 0.0f, 1.0f},
	    .verticalFOV = viewProjection_.fovAngleY,
	    .aspectRatio = viewProjection_.aspectRatio,
	    .nearZ = viewProjection_.nearZ,
	    .farZ = viewProjection_.farZ,
	};

	//ビュー行列の更新
	viewProjection_.UpdateMatrix();

	ImGui::Begin("Camera");
	ImGui::DragFloat3("Rotate", &viewProjection_.rotation_.x, 0.1f);
	ImGui::End();
}

Vector3 FollowCamera::offsetCalculation(const Vector3& offset) const {
	Vector3 offset_ = offset;
	Matrix4x4 newRotateMatrix = matrix_->MakeRotateMatrix(viewProjection_.rotation_);

	offset_ = vector_->TransformNormal(offset_, newRotateMatrix);

	return offset_;
}

void FollowCamera::Reset() {
	if (target_) {
		//追従座標・角度の初期化
		interTarget_ = target_->translation_;
		viewProjection_.rotation_.y = target_->rotation_.y;
	}
	Vector3 offset = offsetCalculation(baseOffset);
	viewProjection_.translation_ = interTarget_ + offset;
}