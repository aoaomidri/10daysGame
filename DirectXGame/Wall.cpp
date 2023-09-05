#include "Wall.h"
void Wall::Initialize(const std::vector<Model*>& models) { 
	BaseField::Initialize(models);
	worldTransform_.translation_.x = 270.0f;
	for (int i = 0; i < 3; i++) {
		worldTransformWall_[i].Initialize();
	}
	worldTransformWall_[0].translation_.x = -270.0f;

	worldTransformWall_[1].rotation_.y = 1.57f;
	worldTransformWall_[1].translation_.z = 270.0f;

	worldTransformWall_[2].rotation_.y = 1.57f;
	worldTransformWall_[2].translation_.z = -270.0f;
}

void Wall::Update() { 
	worldTransform_.UpdateMatrix(scale);
	for (int i = 0; i < 3; i++) {
		worldTransformWall_[i].UpdateMatrix(scale);
	}
}

void Wall::Draw(const ViewProjection& viewProjection) {
	models_[0]->Draw(worldTransform_, viewProjection);
	models_[1]->Draw(worldTransformWall_[0], viewProjection);
	models_[2]->Draw(worldTransformWall_[1], viewProjection);
	models_[3]->Draw(worldTransformWall_[2], viewProjection);
}