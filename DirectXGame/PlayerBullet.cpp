#include "PlayerBullet.h"
#include "Player.h"
#include <assert.h>
#include "MyVector.h"
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
	static MyVector vector;
	
	switch (state_) {
	case PlayerBullet::PlayerBulletState::Idle:
		worldTransform_.translation_ = player_->GetOBB().center;
		deathTimer_ = kLifeTime;
		break;
	case PlayerBullet::PlayerBulletState::Move:
		if (--deathTimer_ <= 0) {
			//isDead_ = true;
			state_ = PlayerBulletState::Return;
			t = 0.0f;
		}

		worldTransform_.AddTransform(velocity_);
		t = 0.0f;
		break;
	case PlayerBullet::PlayerBulletState::Return:
		//Vector3 velocity;


		/*
		worldTransform_.translation_ =
		    vector.Multiply(
		        (t), player_->GetWorldPosition(player_->GetBodyWorldPosition().matWorld_)) +
		    vector.Multiply((1.0f-t), worldTransform_.translation_);
		*/
		ReturnPlayer();

		break;
	default:
		break;
	}
	
	worldTransform_.UpdateMatrix(scale);

}

void PlayerBullet::ReturnPlayer()
{
	static MyVector vector;
	Vector3 toPlayer = player_->GetOBB().center -
	                   worldTransform_.translation_;

	velocity_ = vector.Slerp(velocity_, toPlayer, 0.05f) * kReturnSpeed;
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	Vector3 velocityXZ{velocity_.x, 0.0f, velocity_.z};
	float besage = vector.Length(velocityXZ);
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, besage);

	worldTransform_.AddTransform(velocity_);
	// t += 0.01f;
	float distance = vector.Length(player_->GetOBB().center -
	    worldTransform_.translation_);
	if (distance <= 10.0f) {
		state_ = PlayerBulletState::Idle;
	}
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}

void PlayerBullet::OnCollision() { 
	//isDead_ = true;
	state_ = PlayerBulletState::Return;
}

Vector3 PlayerBullet::GetWorldPosition() {

	Vector3 worldPos(0, 0, 0);

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

