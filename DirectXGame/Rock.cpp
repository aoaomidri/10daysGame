#include "Rock.h"
void Rock::Initialize(const std::vector<Model*>& models) {
	BaseField::Initialize(models);
	worldTransform_.translation_.x = 270.0f;
	worldTransform_.translation_.y = -40.0f;
	for (int i = 0; i < 3; i++) {
		worldTransformRock_[i].Initialize();
	}
	worldTransformRock_[0].translation_.x = -270.0f;

	worldTransformRock_[1].rotation_.y = 1.57f;
	worldTransformRock_[1].translation_.z = 270.0f;

	worldTransformRock_[2].rotation_.y = 1.57f;
	worldTransformRock_[2].translation_.z = -270.0f;

	for (int i = 0; i < 3; i++) {
		worldTransformRock_[i].translation_.y = -40.0f;
	}
}

void Rock::Update() {
	worldTransform_.UpdateMatrix(scale);
	for (int i = 0; i < 3; i++) {
		worldTransformRock_[i].UpdateMatrix(scale);
	}
}

void Rock::Draw(const ViewProjection& viewProjection) {
	models_[0]->Draw(worldTransform_, viewProjection);
	models_[1]->Draw(worldTransformRock_[0], viewProjection);
	models_[2]->Draw(worldTransformRock_[1], viewProjection);
	models_[3]->Draw(worldTransformRock_[2], viewProjection);
}