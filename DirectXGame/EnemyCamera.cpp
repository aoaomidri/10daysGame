#include "EnemyCamera.h"
#include<imgui.h>
void EnemyCamera::Initialize() {
	viewProjection_.farZ = 70.0f;
	viewProjection_.nearZ = 0.1f;
	viewProjection_.aspectRatio = static_cast<float>(18 / 9);
	viewProjection_.Initialize();
	destinationAngleX_ = 0.1f;

	vector_ = std::make_unique<MyVector>();
	matrix_ = std::make_unique<MyMatrix>();

}

void EnemyCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}

void EnemyCamera::Update() {

	destinationAngleY_ = target_->rotation_.y;		

	viewProjection_.rotation_.y = destinationAngleY_;
	viewProjection_.rotation_.x =
	    vector_->LerpShortAngle(viewProjection_.rotation_.x, destinationAngleX_, 0.1f);

	/*ImGui::Begin("Camera");
	ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.01f);
	ImGui::End();*/

	if (target_) {
		// 追従座標の補完
		interTarget_ = vector_->Lerp(interTarget_, target_->translation_, t);
		// 追従対象からカメラまでのオフセット
		Vector3 offset = offsetCalculation();
		// 座標をコピーしてオフセット分ずらす
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
	// ビュー行列の更新
	viewProjection_.UpdateMatrix();
}

Vector3 EnemyCamera::offsetCalculation() const {
	Vector3 offset = {0.0f, 5.0f, 5.0f};
	Matrix4x4 newRotateMatrix = matrix_->MakeRotateMatrix(viewProjection_.rotation_);

	offset = vector_->TransformNormal(offset, newRotateMatrix);

	return offset;
}

void EnemyCamera::Reset() {
	if (target_) {
		// 追従座標・角度の初期化
		interTarget_ = target_->translation_;
		viewProjection_.rotation_.y = target_->rotation_.y;
	}
	Vector3 offset = offsetCalculation();
	viewProjection_.translation_ = interTarget_ + offset;
}