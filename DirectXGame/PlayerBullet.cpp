#include "PlayerBullet.h"
#include "Player.h"
#include <assert.h>
#include "MyVector.h"
PlayerBullet::~PlayerBullet() {
}

void PlayerBullet::Initialize(
    Model* model,Model* model2, const Vector3& position, const Vector3& rotate, const Vector3& velocity) {
	assert(model);
	
	model_ = model;
	//model++;
	modelFin_ = model2;
	textureHandle_ = TextureManager::Load("black.png");

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	worldTransform_.rotation_ = rotate;

	velocity_ = velocity;

	worldTransformFin_.Initialize();
	worldTransformFin_.translation_ = Fin_offset_Base;
	worldTransformFin_.parent_ = &worldTransform_;
}

void PlayerBullet::Update() {
	static MyVector vector;
	
	switch (state_) {
	case PlayerBullet::PlayerBulletState::Idle:
		Idle();
		break;
	case PlayerBullet::PlayerBulletState::Move:
		Move();
		break;
	case PlayerBullet::PlayerBulletState::Return:
		ReturnPlayer();
		break;
	default:
		break;
	}
	
	worldTransform_.UpdateMatrix(scale);
	FinAnimationUpdate();
	worldTransformFin_.UpdateMatrix(finScale);
}


void PlayerBullet::Idle() 
{
	worldTransform_.translation_ = player_->GetOBB().center;
	//worldTransform_.parent_ = &player_->GetWorldTransform();
	
	/*
	theta += float(M_PI) / 120.0f;
	static float n = 3.0f;
	static float d = 2.0f;
	float a = n / d;
	worldTransform_.translation_.x += (sinf(a * theta)) * cosf(theta) * 5.0f;
	worldTransform_.translation_.y += 2.0f;
	worldTransform_.translation_.z += (sinf(a * theta)) * sinf(theta) * 5.0f - 3.0f;
	*/
	//worldTransform_.translation_.y += 2.0f;
	//worldTransform_.translation_.z += -12.0f;
	deathTimer_ = kLifeTime;
}

void PlayerBullet::Move()
{
	if (--deathTimer_ <= 0) {
		// isDead_ = true;
		//state_ = PlayerBulletState::Return;
		t = 0.0f;
	}

	static MyVector vector;
	Vector3 toEnemy = target_->translation_ - worldTransform_.translation_;

	velocity_ = vector.Slerp(velocity_, toEnemy, 0.05f) * kReturnSpeed;
	
	
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	Vector3 velocityXZ{velocity_.x, 0.0f, velocity_.z};
	float besage = vector.Length(velocityXZ);
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, besage);

	worldTransform_.AddTransform(velocity_);
	t = 0.0f;
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
	modelFin_->Draw(worldTransformFin_,viewProjection);
}

void PlayerBullet::OnCollision() { 
	//isDead_ = true;
	if (state_ == PlayerBulletState::Move)
	{
		velocity_.x *= -1.0f;
		velocity_.y *= -1.0f;
		velocity_.z *= -1.0f;
	}
	else
	{
		isDead_ = true;
	}
	state_ = PlayerBulletState::Return;
}

Vector3 PlayerBullet::GetWorldPosition() {

	Vector3 worldPos(0, 0, 0);

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

void PlayerBullet::FinAnimationUpdate()
{ 
	finRotate += 0.5f;
	worldTransformFin_.rotation_.y = (std::sin(finRotate))*float(M_PI)/4.0f; 
}