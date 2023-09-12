#include "GameCamera.h"
#include <imgui.h>

void GameCamera::Initialize() {
	viewProjection_.farZ = 2000.0f;
	viewProjection_.nearZ = 0.01f;
	viewProjection_.translation_ = {0.0f, 0.0f, 0.0f};
	viewProjection_.rotation_ = {0.0f};
	viewProjection_.Initialize();
	destinationAngleX_ = 0.2f;

	distance = -140.0f;

	shotOffset = {2.0f, 6.0f, -7.5f};

	rootOffset = {0.0f, 3.0f, distance};

	nowRotate = 0.0f;

	rotateSpeed = 0.005f;

	minRotate = -0.31f;
	maxRotate = 6.28f;

	baseOffset = rootOffset;

	isSetRotate = true;

	target_ = nullptr;

	vector_ = std::make_unique<MyVector>();
	matrix_ = std::make_unique<MyMatrix>();

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void GameCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	if (isSetRotate) {

		viewProjection_.rotation_.y = 3.14f + target_->rotation_.y;
		isSetRotate = false;
	}
	//Reset();
}

void GameCamera::Update() {
	

	/*viewProjection_.rotation_.y =
	    vector_->LerpShortAngle(viewProjection_.rotation_.y, destinationAngleY_, 0.1f);
	viewProjection_.rotation_.x =
	    vector_->LerpShortAngle(viewProjection_.rotation_.x, destinationAngleX_, 0.1f);*/

	if (target_) {
		rotateSpeed = 0.01f;

		// 追従座標の補完
		interTarget_ = vector_->Lerp(interTarget_, target_->translation_, t);
		// 追従対象からカメラまでのオフセット
		Vector3 offset = offsetCalculation(baseOffset);

		// 座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = interTarget_ + offset;

		if (vector_->Length(interTarget_-target_->translation_)<=1.0f) {
			if (nowRotate<=maxRotate) {
				nowRotate += rotateSpeed;
				viewProjection_.rotation_.y += rotateSpeed;
				
			}
		}

	} else {
		viewProjection_.rotation_.y += rotateSpeed;
		rotateSpeed = 0.005f;
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

	// ビュー行列の更新
	viewProjection_.UpdateMatrix();

}

void GameCamera::DrawImgui() {
#ifdef _DEBUG
	ImGui::Begin("GameCamera");
	ImGui::DragFloat3("Translation", &viewProjection_.translation_.x, 0.1f);
	ImGui::DragFloat3("Rotate", &viewProjection_.rotation_.x, 0.1f);
	ImGui::DragFloat3("offset", &baseOffset.x, 0.1f);

	ImGui::DragFloat("NowRotate", &nowRotate, 0.1f);
	ImGui::DragFloat("RotateSpeed", &rotateSpeed, 0.1f);
	ImGui::End();
#endif

	
}

Vector3 GameCamera::offsetCalculation(const Vector3& offset) const {
	Vector3 offset_ = offset;
	Matrix4x4 newRotateMatrix = matrix_->MakeRotateMatrix(viewProjection_.rotation_);

	offset_ = vector_->TransformNormal(offset_, newRotateMatrix);

	return offset_;
}

void GameCamera::Reset() {
	if (target_) {
		// 追従座標・角度の初期化
		interTarget_ = target_->translation_;
		viewProjection_.rotation_.y = target_->rotation_.y;
	}
	Vector3 offset = offsetCalculation(baseOffset);
	viewProjection_.translation_ = interTarget_ + offset;
}