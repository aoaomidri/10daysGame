#include "PlayerBullet.h"
#include <assert.h>
PlayerBullet::~PlayerBullet() {
}

void PlayerBullet::Initialize(
    Model* model, const Vector3& position, const Vector3& rotate, const Vector3& velocity) {
	assert(model);

	model_ = model;

	textureHandle_ = TextureManager::Load("black.png");

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	worldTransform_.rotation_ = rotate;

	velocity_ = velocity;
}

void PlayerBullet::Update() {

	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}


	worldTransform_.AddTransform(velocity_);

	worldTransform_.UpdateMatrix(scale);

}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}

void PlayerBullet::OnCollision() { isDead_ = true; }

Vector3 PlayerBullet::GetWorldPosition() {

	Vector3 worldPos(0, 0, 0);

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

