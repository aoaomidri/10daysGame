#include "PlayerBullet.h"
#include "Player.h"
#include <assert.h>
#include "MyVector.h"
PlayerBullet::~PlayerBullet() {
}

void PlayerBullet::Initialize(
    Model* model,Model* model2, Model* model3, const Vector3& position, const Vector3& rotate, const Vector3& velocity) {
	assert(model);
	
	model_ = model;
	//model++;
	modelFin_ = model2;
	modelWaterFlow_ = model3;

	textureHandle_ = TextureManager::Load("black.png");

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	worldTransform_.rotation_ = rotate;

	velocity_ = velocity;

	worldTransformRoll_.Initialize();
	worldTransformRoll_.parent_ = &worldTransform_;

	worldTransformFin_.Initialize();
	worldTransformFin_.translation_ = Fin_offset_Base;
	worldTransformFin_.parent_ = &worldTransformRoll_;

	waterFlowEffect.Initialize(modelWaterFlow_);
	waterFlowEffect.SetEmitterParent(&worldTransform_);
	waterFlowEffect.SetBulletVelocity(&velocity_);

	worldTransformHerd_.Initialize();
	worldTransformHerd_.translation_.x = (float(rand()) / float(RAND_MAX) - 0.5f) * 10.0f;
	worldTransformHerd_.translation_.y = 2.0f + (float(rand()) / float(RAND_MAX) - 0.5f) * 10.0f;
	worldTransformHerd_.translation_.z = -11.0f;
}

void PlayerBullet::SetPlayer(Player* player) {
	player_ = player; 
	//worldTransform_.parent_ = &player->GetWorldTransform();
};

void PlayerBullet::Update() {
	static MyVector vector;
	
	switch (state_) {
	case PlayerBullet::PlayerBulletState::Idle:
		Idle();
		waterFlowEffect.SetIsDraw(false);
		waterFlowEffect.SetIsPop(false);
		break;
	case PlayerBullet::PlayerBulletState::Stance:
		Stance();
		deathTimer_ = kLifeTime;
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
	waterFlowEffect.SetBulletPos(worldTransform_.translation_);
	waterFlowEffect.Update();



	worldTransform_.UpdateMatrix(scale);
	worldTransformRoll_.UpdateMatrix(finScale);
	FinAnimationUpdate();
	worldTransformFin_.UpdateMatrix(finScale);
	worldTransformHerd_.UpdateMatrix(finScale);
}


void PlayerBullet::Idle() 
{
	worldTransformHerd_.parent_ = &player_->GetWorldTransform(); 
	target_ = &worldTransformHerd_;
	
	static MyVector vector;
	static MyMatrix matrix;
	
	velocity_ = GetTargetWorldPosition() - worldTransform_.translation_;
	velocity_ = vector.Multiply(idleFollow,velocity_);
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	Vector3 velocityXZ{velocity_.x, 0.0f, velocity_.z};
	float besage = vector.Length(velocityXZ);
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, besage);

	worldTransform_.AddTransform(velocity_);
}

void PlayerBullet::Stance() {

}

void PlayerBullet::Move()
{
	if (--deathTimer_ <= 0) {
		// isDead_ = true;
		state_ = PlayerBulletState::Return;
		t = 0.0f;
	}
	static MyVector vector;
	static MyMatrix matrix;
	if (player_->GetcheckCamera() == 1) {
	
	Vector3 toEnemy = enemy_->translation_ - worldTransform_.translation_;	

		velocity_ = vector.Slerp(velocity_, toEnemy, 0.05f) * kAttackSpeed;
	}


	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	Vector3 velocityXZ{velocity_.x, 0.0f, velocity_.z};
	float besage = vector.Length(velocityXZ);
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, besage);
	/*
	velocityXZ = {velocity_.x, velocity_.y, 0.0f};
	besage = vector.Length(velocityXZ);
	worldTransform_.rotation_.z = std::atan2(float(M_PI)/2.0f, besage);
	*/
	worldTransformRoll_.rotation_.z += float(M_PI)/8.0f;
	waterFlowEffect.SetIsDraw(true);
	waterFlowEffect.SetIsPop(true);

	worldTransform_.AddTransform(velocity_);
	
	t = 0.0f;
}

void PlayerBullet::ReturnPlayer()
{
	static MyVector vector;
	Vector3 toPlayer = GetTargetWorldPosition() -
	                   worldTransform_.translation_;

	velocity_ = vector.Slerp(velocity_, toPlayer, 0.05f) * kReturnSpeed;
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	Vector3 velocityXZ{velocity_.x, 0.0f, velocity_.z};
	float besage = vector.Length(velocityXZ);
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, besage);

	worldTransform_.AddTransform(velocity_);


	worldTransformRoll_.rotation_.z = 0.0f;
	

	float distance = vector.Length(GetTargetWorldPosition() -
	    worldTransform_.translation_);
	if (distance <= 10.0f) {
		state_ = PlayerBulletState::Idle;
		/*
		worldTransform_.translation_.x = (float(rand()) / float(RAND_MAX) - 0.5f) * 10.0f;
		worldTransform_.translation_.y = 2.0f + (float(rand()) / float(RAND_MAX) - 0.5f) * 10.0f;
		worldTransform_.translation_.z = -11.0f;
		worldTransform_.rotation_.x = 0.0f;
		worldTransform_.rotation_.y = 0.0f;
		worldTransform_.rotation_.z = 0.0f;
		//worldTransform_.parent_ = &player_->GetWorldTransform();
		*/
	}
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransformRoll_, viewProjection);
	modelFin_->Draw(worldTransformFin_,viewProjection);
	waterFlowEffect.Draw(viewProjection);
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

	//worldPos.x = worldTransform_.translation_.x;
	//worldPos.y = worldTransform_.translation_.y;
	//worldPos.z = worldTransform_.translation_.z;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

Vector3 PlayerBullet::GetTargetWorldPosition() {
	Vector3 worldPos(0, 0, 0);
	worldPos.x = target_->matWorld_.m[3][0];
	worldPos.y = target_->matWorld_.m[3][1];
	worldPos.z = target_->matWorld_.m[3][2];

	return worldPos;
}

void PlayerBullet::FinAnimationUpdate()
{ 
	finRotate += 0.5f;
	worldTransformFin_.rotation_.y = (std::sin(finRotate))*float(M_PI)/4.0f; 
}

void PlayerBullet::SetShot(const Vector3& position, const Vector3& rotate, const Vector3& velocity)
{
	worldTransform_.translation_ = position;
	worldTransform_.translation_ = GetWorldPosition();

	worldTransform_.rotation_ = rotate;
	velocity_ = velocity;
	worldTransform_.parent_ = nullptr;
};

void PlayerBullet::SetShotIdle(const Vector3& position) {
	state_ = PlayerBulletState::Stance;
	worldTransform_.translation_ = position;
	worldTransform_.translation_.x = 0.0f;
	worldTransform_.translation_.y += 3.0f;
	worldTransform_.translation_.z = 0.0f;
	worldTransform_.rotation_.x = -float(M_PI) / 2.0f;
	worldTransform_.rotation_.y = 0.0f;
	worldTransform_.rotation_.z = 0.0f;
	worldTransform_.parent_ = &player_->GetLArmWorldTransform();
	// worldTransform_.rotation_ = rotate;
	// worldTransform_.parent_ = nullptr;
};

void PlayerBullet::StanceCancel() {
	state_ = PlayerBulletState::Idle;
	worldTransform_.parent_ = nullptr;
	Vector3 position = GetWorldPosition();
	worldTransform_.translation_ = position;
	/* worldTransform_.translation_.x = (float(rand()) / float(RAND_MAX) - 0.5f) * 10.0f;
	worldTransform_.translation_.y = 2.0f + (float(rand()) / float(RAND_MAX) - 0.5f) * 10.0f;
	worldTransform_.translation_.z = -11.0f;
	worldTransform_.rotation_.x = 0.0f;
	worldTransform_.rotation_.y = 0.0f;
	worldTransform_.rotation_.z = 0.0f;
	worldTransform_.parent_ = &player_->GetWorldTransform();
	*/
}
