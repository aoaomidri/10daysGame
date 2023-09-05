#include "EnemyBullet.h"
#include<assert.h>
EnemyBullet::~EnemyBullet() {}

void EnemyBullet::Initialize(
    Model* model, const Vector3& position, const Vector3& rotate, const Vector3& velocity) {
	assert(model);

	model_ = model;

	textureHandle_ = TextureManager::Load("black.png");

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	worldTransform_.rotation_ = rotate;

	velocity_ = velocity;
}

void EnemyBullet::Update() {

	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	if (worldTransform_.translation_.y<-15.0f) {
		isDead_ = true;
	}

	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);

	minusVelocity = {0, -worldTransform_.rotation_.y, 0};

	minusMatrix = matrix_.MakeRotateMatrixY(minusVelocity);

	velocityZ = vector_.TransformNormal(velocity_, minusMatrix);

	worldTransform_.rotation_.x = std::atan2(-velocityZ.y, velocityZ.z);

	worldTransform_.AddTransform(velocity_);

	worldTransform_.UpdateMatrix(scale);
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void EnemyBullet::OnCollision() { isDead_ = true; }

Vector3 EnemyBullet::GetWorldPosition() {

	Vector3 worldPos(0, 0, 0);

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}