#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include <list>

class Player;

class PlayerBullet {
public:
	enum class PlayerBulletState {
		Idle,
		Move,
		Return
	};
	~PlayerBullet();

	void Initialize(
	    Model* model, Model* model2, const Vector3& position, const Vector3& rotate,
	    const Vector3& velocity);

	void Update();

	void FinAnimationUpdate();

	void Move();

	void ReturnPlayer();

	void Idle();

	void Draw(const ViewProjection& viewProjection);

	bool IsDead() const { return isDead_; }

	void setTarget(const WorldTransform* target) { target_ = target; };

	Vector3 GetWorldPosition();
	inline PlayerBulletState GetState() { return state_; };
	inline void SetState(PlayerBulletState state) { state_ = state; };
	inline void SetPlayer(Player* player) { player_ = player; };
	void SetShot(const Vector3& position, const Vector3& rotate, const Vector3& velocity);

	// 衝突したら呼び出す関数
	void OnCollision();

	float radius = 1.0f;

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	Model* modelFin_ = nullptr;
	
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	float size = 1.8f;
	float finSize = 1.0f;

	Vector3 scale = {size, size, size};
	Vector3 finScale = {finSize, finSize, finSize};

	Vector3 velocity_;
	// 寿命
	static const int32_t kLifeTime = 60;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	PlayerBulletState state_ = PlayerBulletState::Idle;

	Player* player_ = nullptr;

	float t = 0.0f;

	//攻撃時の速度
	const float kAttackSpeed =2.0f;

	//戻ってくるときの速度
	const float kReturnSpeed = 1.2f;

	Vector3 Fin_offset_Base = {0.0f, 0.0f, -1.0f};
	WorldTransform worldTransformFin_;
	float finRotate = 0.0f;
	float theta=0.0f;

	const WorldTransform* target_ = nullptr;
};
